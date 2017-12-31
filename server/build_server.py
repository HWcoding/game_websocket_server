#!/usr/bin/env python

# Options:
#	debug
#		builds a debug build (this is the default)
#	release
#		builds a release build
#	clean
#		runs 'make clean' to remove temparary objs
#	weak
#		uses weaker warnings while building
#	docs
#		builds documentation
#	cov
#		builds coverage data
#	tidy
#		runs clang-tidy static analyzer


import sys
import os
import subprocess
import shutil
import multiprocessing



# build flags
#" -Weverything -Wno-padded -Wno-c++98-compat -Wno-weak-vtables -Wno-unused-exception-parameter"

StrongWarnings = " -Winvalid-pch -fmax-errors=5 -Wall -Wextra -Wformat=2 -Wformat-signedness " +\
"-Wmissing-include-dirs -Wswitch-default -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wconversion " +\
"-Wuseless-cast -Wsign-conversion -Wfloat-conversion -Wlogical-op -Wmissing-declarations -pedantic " +\
"-ansi -Weffc++ -Wno-odr -Werror"
WeakWarnings = " -Wall -Wextra -pedantic -ansi -Weffc++ -Wno-odr"
TestWarnings = " -Wall -Wno-odr"

ReleaseOptimizations = " -Ofast -falign-functions=16 -falign-loops=16 -march=native"
DebugBuild = "CFLAGS=-DDEBUG -g3 -fno-omit-frame-pointer -fno-inline -fprofile-arcs -ftest-coverage"
ReleaseBuild = "CFLAGS=-DNDEBUG -flto " + ReleaseOptimizations



# checks if input is a command line argument
def getIsArgument( argument ):
	for arg in sys.argv:
		if arg.lower() == argument:
			return True
	return False


# returns true if temparary files should be deleted. Set by passing clean as a command argument
def getCleanFlag():
	return getIsArgument( "clean" )


# echos input inside a blue gradiant bar
def printMajorHeader( text ):
	print("\n\033[48;5;17m "+
	"\033[48;5;18m "+
	"\033[48;5;19m "+
	"\033[48;5;20m "+
	"\033[1;37;48;5;21m "+
	"                  "+
	text+
	"\033[48;5;21m "+
	"                  "+
	"\033[48;5;20m "+
	"\033[48;5;19m "+
	"\033[48;5;18m "+
	"\033[48;5;17m "+
	"\033[0m")


# echos the input with a green background
def printMinorHeader( text ):
	print("\n\033[1;37;48;5;17m   "+
	text+
	"   "+
	"\033[0m")

# applies a green color to text
def highlight( text ):
	return "\033[92m"+text+"\033[0m"

# echos input with green formating
def printHighlight( text ):
	print(highlight( text ))

# makes directory dir if it does not exist
def makeDir( dir ):
	if not os.path.isdir(dir):
		os.makedirs(dir)


def compileCppcheck():
	if not os.path.isfile( "./cppcheck_built" ):
		os.chdir( "./cppcheck_source" )

		print("")
		printHighlight( "Compiling Cppcheck" )

		cppFileslist = []
		for root, dirs, files in os.walk("./externals/simplecpp/"):
			for file in files:
				if file.endswith('.cpp'):
					fullpath = os.path.join(root, file)
					cppFileslist.append(fullpath)

		for root, dirs, files in os.walk("./externals/tinyxml/"):
			for file in files:
				if file.endswith('.cpp'):
					fullpath = os.path.join(root, file)
					cppFileslist.append(fullpath)

		for root, dirs, files in os.walk("./cli/"):
			for file in files:
				if file.endswith('.cpp'):
					fullpath = os.path.join(root, file)
					cppFileslist.append(fullpath)

		for root, dirs, files in os.walk("./lib/"):
			for file in files:
				if file.endswith('.cpp'):
					fullpath = os.path.join(root, file)
					cppFileslist.append(fullpath)


		command = ["g++"]
		opt = ReleaseOptimizations.strip()
		optimizations = opt.split(" ")
		for item in optimizations:
			command.append(item)
		command.append("-DNDEBUG")
		command.append("-std=c++11")
		command.append("-o")
		command.append("cppcheck")
		command.append("-Iexternals/simplecpp")
		command.append("-Iexternals/tinyxml")
		command.append("-Ilib")

		for item in cppFileslist:
			command.append(item)

		make_process =	subprocess.Popen(command, stderr=subprocess.STDOUT)

		os.chdir( "../" )
	return make_process


def copyCppcheckfiles():
	if not os.path.isfile( "./cppcheck_built" ):
		shutil.copy( "./cppcheck_source/cppcheck", "./cppcheck/cppcheck" )
		shutil.copy( "./cppcheck_source/AUTHORS", "./cppcheck/cppcheck_AUTHORS" )
		shutil.copy( "./cppcheck_source/COPYING", "./cppcheck/cppcheck_LICENSE" )
		shutil.copy( "./cppcheck_source/readme.md", "./cppcheck/cppcheck_readme.md" )
		shutil.copy( "./cppcheck_source/readme.txt", "./cppcheck/cppcheck_readme.txt" )
		shutil.copy( "./cppcheck_source/cfg/std.cfg", "./cppcheck/std.cfg" )

		os.remove( "./cppcheck_downloaded" )
		shutil.rmtree("./cppcheck_source")
		#touch cppcheck_built
		os.close(os.open("./cppcheck_built", os.O_CREAT|os.O_EXCL))




#download Cppcheck code analysis tool
def downloadCppcheck():
	if not os.path.isfile( "./cppcheck_built" ):
		printMinorHeader( "Building Cppcheck" )

		if os.path.isdir("./cppcheck"):
			shutil.rmtree("./cppcheck")
		os.makedirs( "./cppcheck" )

		if not os.path.isfile( "./cppcheck_downloaded"):
			if os.path.isdir("./cppcheck_source" ):
				shutil.rmtree("./cppcheck_source")
			os.makedirs( "./cppcheck_source" )

			print("")
			printHighlight( "Cloning Cppcheck code analysis tool from github at:\nhttps://github.com/danmar/cppcheck.git" )
			#git clone https://github.com/danmar/cppcheck.git cppcheck_source

			subprocess.call(["git", "clone", "https://github.com/danmar/cppcheck.git",
								"cppcheck_source"],
								stderr=subprocess.STDOUT)

			#touch cppcheck_downloaded
			os.close(os.open("./cppcheck_downloaded", os.O_CREAT|os.O_EXCL))


def downloadGoogleTest():
	if not os.path.isfile( "./google_test_built" ):
		printMinorHeader( "Building Google Test" )
		if os.path.isdir("./google_test"):
			shutil.rmtree("./google_test")
		os.makedirs( "./google_test" )
		os.chdir( "./google_test" )
		os.makedirs( "./include" )
		os.chdir( "../" )
		if not os.path.isfile( "./google_test_and_mock_downloaded"):
			if os.path.isdir("./google_test_and_mock" ):
				shutil.rmtree("./google_test_and_mock")
			os.makedirs( "./google_test_and_mock" )
			print("")
			printHighlight( "Cloning googletest from github at:\nhttps://github.com/google/googletest.git" )
			#git clone https://github.com/google/googletest.git google_test_and_mock
			subprocess.call(["git", "clone", "https://github.com/google/googletest.git",
								"google_test_and_mock"],
								stderr=subprocess.STDOUT)
			os.close(os.open("./google_test_and_mock_downloaded", os.O_CREAT|os.O_EXCL))


def compileGoogleTest():
	os.chdir( "./google_test_and_mock" )
	gtestDir="./googletest"
	gmockDir="./googlemock"
	print("")
	printHighlight( "Compiling Google Test" )

	opt = ReleaseOptimizations.strip()
	optimizations = opt.split(" ")

	command1 = ["g++", "-isystem", gtestDir+"/include", "-I"+gtestDir,
		"-isystem", gmockDir+"/include", "-I"+gmockDir,
		"-pthread"]
	for item in optimizations:
		command1.append(item)
	command1.append("-c")
	command1.append(gtestDir+"/src/gtest-all.cc")


	command2 = ["g++", "-isystem", gtestDir+"/include", "-I"+gtestDir,
		"-isystem", gmockDir+"/include", "-I"+gmockDir,
		"-pthread"]
	for item in optimizations:
		command2.append(item)
	command2.append("-c")
	command2.append(gmockDir+"/src/gmock-all.cc")

	command3 = ["gcc-ar", "-rv", "../google_test/google_test.a", "gtest-all.o", "gmock-all.o"]

	make_process1 =	subprocess.Popen(command1, stderr=subprocess.STDOUT)
	make_process2 =	subprocess.Popen(command2, stderr=subprocess.STDOUT)

	if make_process1.wait() != 0:
			raise Exception
	if make_process2.wait() != 0:
			raise Exception

	subprocess.call(command3,stderr=subprocess.STDOUT)

	os.chdir( "../")


	GLIB_SOURCE="./google_test_and_mock"
	GLIB_DEST="./google_test"

	shutil.copytree(GLIB_SOURCE+"/googletest/include/gtest", GLIB_DEST+"/include/gtest")
	shutil.copytree(GLIB_SOURCE+"/googlemock/include/gmock", GLIB_DEST+"/include/gmock")
	shutil.copy(GLIB_SOURCE+"/README.md", GLIB_DEST+"/googletest_README.md")
	shutil.copy(GLIB_SOURCE+"/googletest/LICENSE", GLIB_DEST+"/googletest_LICENSE")
	shutil.copy(GLIB_SOURCE+"/googlemock/LICENSE", GLIB_DEST+"/googlemock_LICENSE")
	shutil.copy(GLIB_SOURCE+"/googletest/CONTRIBUTORS", GLIB_DEST+"/googletest_CONTRIBUTORS")
	shutil.copy(GLIB_SOURCE+"/googlemock/CONTRIBUTORS", GLIB_DEST+"/googlemock_CONTRIBUTORS")

	#rm google_test_and_mock_downloaded
	os.remove( "./google_test_and_mock_downloaded" )

	#rm -rf ./google_test_and_mock
	shutil.rmtree("./google_test_and_mock")

	#touch google_test_built
	os.close(os.open("./google_test_built", os.O_CREAT|os.O_EXCL))
	printHighlight( "Google test finished")



# builds external dependancies
def buildExternals():
	current = os.getcwd()
	makeDir("../external")

	os.chdir( "../external" )
	Cppcheck_process = None
	if not os.path.isfile( "./cppcheck_built" ):
		downloadCppcheck()
		Cppcheck_process = compileCppcheck()
	if not os.path.isfile( "./google_test_built" ):
		downloadGoogleTest()
		compileGoogleTest()
	printHighlight( "Waiting for packages to finish compiling" )


	if Cppcheck_process is not None:
		if Cppcheck_process.wait() != 0:
			raise SystemExit
		copyCppcheckfiles()

	printHighlight( "Finished" )
	#buildGoogleTest


	#wait # for all packages to compile
	os.chdir( current )



# creates necessary directories
def buildDirectories():

	makeDir("./source")
	makeDir("./tests")
	makeDir("./analysis")
	makeDir("./docs")

	makeDir("./objs")
	os.chdir( "./objs" )
	makeDir("./debug")
	makeDir("./release")
	os.chdir( "../" )

	makeDir("./deps")
	os.chdir( "./deps" )
	makeDir("./debug")
	os.chdir( "./debug" )
	makeDir("./temp")
	os.chdir( "../" )

	makeDir("./release")
	os.chdir( "./release" )
	makeDir("./temp")
	os.chdir( "../../tests" )

	makeDir("./source")
	makeDir("./bin")
	os.chdir( "./bin" )
	makeDir("./debug")
	makeDir("./release")
	os.chdir( "../" )

	makeDir("./objs")
	os.chdir( "./objs" )
	makeDir("./debug")
	makeDir("./release")
	os.chdir( "../" )

	makeDir("./gcov")

	makeDir("./deps")
	os.chdir( "./deps" )
	makeDir("./debug")
	os.chdir( "./debug" )
	makeDir("./temp")
	os.chdir( "../" )
	makeDir("./release")
	os.chdir( "./release" )
	makeDir("./temp")
	os.chdir( "../../../" )


# prints the number of lines contained in .cpp and .h files in ./source
def printProjectLineCount():
	# get line count
	os.chdir( "./source" )

	ProductionLines = 0

	for root, dirs, files in os.walk("./"):
		for file in files:
			if file.endswith('.cpp') or file.endswith('.h'):
				fullpath = os.path.join(root, file)
				with open(fullpath) as f:
					ProductionLines += sum(1 for _ in f)

	os.chdir( "../tests" )

	TestLines = 0

	for root, dirs, files in os.walk("./"):
		for file in files:
			if file.endswith('.cpp') or file.endswith('.h'):
				fullpath = os.path.join(root, file)
				with open(fullpath) as f:
					TestLines += sum(1 for _ in f)

	os.chdir( "../" )

	print( highlight( "Project contains: ") + str(ProductionLines) + " lines of production code")
	print( highlight( "Project contains: ") + str(TestLines) + " lines of test code")
	print( highlight( "Total is:         ") + str(TestLines + ProductionLines) + " lines of code")

#determine if a package is installed
def isNotInstalled( program ):
	try:
		with open(os.devnull) as devnull:
			subprocess.Popen([program], stdin=devnull, stderr=devnull, stdout=devnull).communicate()
	except OSError as error:
		if error.errno == os.errno.ENOENT:
			return True
	return False


def codeAnalysis():
	if getIsArgument("clean") == False :
		make_process = subprocess.Popen(["../external/cppcheck/cppcheck",
			"--quiet", "-I./", "--enable=warning,performance,information",
			"-j8", "--cppcheck-build-dir=./analysis", "./source"], stderr=subprocess.STDOUT)
		if make_process.wait() != 0:
			raise Exception

		if getIsArgument( "tidy" ):
			if isNotInstalled( "clang-tidy" ):
				printHighlight( "Error: package clang-tidy is not installed" )
			else:
				os.chdir( "./source" )
				for root, dirs, files in os.walk("./"):
					for file in files:
						if file.endswith('.cpp'):
							fullpath = os.path.join(root, file)
							make_process = subprocess.Popen(["clang-tidy", fullpath,
								"-extra-arg=-I../", "-extra-arg=-std=c++14",
								"-checks=modernize-*,clang-analyzer-*,clang-analyzer-alpha.deadcode.UnreachableCode,-clang-analyzer-alpha.core.CastToStruct"],
								stderr=subprocess.STDOUT)
							if make_process.wait() != 0:
								raise Exception
				os.chdir( "../" )
	else:
		if os.path.isfile("./analysis/files.txt"):
			#rm ./analysis*
			if os.path.isdir("./analysis" ):
				shutil.rmtree("./analysis")
				os.makedirs( "./analysis" )

# returns a string containing the build flags to use with makefile

def getBuildFlag():
	if getIsArgument( "release" ):
		BuildFlag = ReleaseBuild
	else:
		BuildFlag = DebugBuild

	return BuildFlag



# returns a string containing the build flags to use with the production makefile including warnings
def getProductionBuildFlag():
	ProductionBuildFlag = getBuildFlag()
	if getIsArgument( "weak" ):
		ProductionBuildFlag += WeakWarnings
	else:
		ProductionBuildFlag += StrongWarnings

	return ProductionBuildFlag

# returns a string describing the build type.
def getBuildType():
	if getIsArgument( "release" ):
		BuildType = "release"
	else:
		BuildType = "debug"

	return BuildType



# compiles all the .cpp files in the ./source directory
def compileProductionFiles():
	# move to source directory
	os.chdir( "./source" )

	# get list of cpp files to compile and store in FileList.
	cppFiles = ""
	for root, dirs, files in os.walk("./"):
		for file in files:
			if file.endswith('.cpp'):
				fullpath = os.path.join(root, file)
				cppFiles += "" + fullpath + " "
	FileList = "FILE_LIST=" + cppFiles

	BuildOptions= getProductionBuildFlag()

	if getCleanFlag():
		BuildOptions = "clean"

	# feed the list to make and build program
	if getBuildType() != "debug":
		make_process = subprocess.Popen(["make", "-s", "-j8", "LINK_TYPE=lto", BuildOptions, FileList], stderr=subprocess.STDOUT)
	else:
		make_process = subprocess.Popen(["make", "-s", "-j8", BuildOptions, FileList], stderr=subprocess.STDOUT)

	if make_process.wait() != 0:
		raise SystemExit
	os.chdir( "../")


def compileTestLibs():
	# move to source directory
	os.chdir( "./tests/test_lib" )

	# get list of cpp files to compile and store in FileList.
	cppFiles = ""
	for root, dirs, files in os.walk("./"):
		for file in files:
			if file.endswith('.cpp'):
				fullpath = os.path.join(root, file)
				cppFiles += "" + fullpath + " "
	FileList = "FILE_LIST=" + cppFiles

	BuildOptions = getProductionBuildFlag()

	if getCleanFlag():
		BuildOptions="clean"

	# feed the list to make and build program
	make_process = subprocess.Popen(["make", "-s", "-j8", BuildOptions, FileList], stderr=subprocess.STDOUT)
	if make_process.wait() != 0:
		raise Exception
	os.chdir( "../../" )



# returns a string containing the build flags to use with the test makefile including warnings
def getTestBuildFlag():
	if getIsArgument( "release" ):
		TestBuildFlag = ReleaseBuild
	else:
		TestBuildFlag = DebugBuild

	TestBuildFlag += TestWarnings
	return TestBuildFlag



# takes a test .cpp filename as an argument and returns dependencies listed in it
def getTestDependencies( file ):

	Dependencies=""

	readingDependency = False
	with open(file, "r") as f:
		for line in f:
			line = line.rstrip()
			# check to see if the line begins with our macro
			lineBegin = line[:7]
			if lineBegin == "#define":
				lineBegin = line[7:]
				lineBegin = lineBegin.lstrip()
				if lineBegin[:27] == "TEST_FILE_LINK_DEPENDENCIES":
					readingDependency= True
					line = lineBegin[27:]

			# If variable is yes, append
			if readingDependency:
				# if line does not end in \ we have finished
				lastChar = line[-1]
				if lastChar != "\\":
					readingDependency = False
				else:
					# remove ending \
					line = line[:-1]
				Dependencies += line.strip()

	# remove the ending quote
	Dependencies = Dependencies[:-1]
	# replace the beginning quote
	Dependencies = Dependencies[1:]
	# replace commas with spaces
	Dependencies = Dependencies.replace(',', ' ')

	return Dependencies



# takes a test .cpp filename as argument and compiles it
def compileTest(file):
	ObjectList = "OBJECT_LIST=" + getTestDependencies( file )
	Cov = "COVERAGE=0"
	if getIsArgument( "cov" ):
		Cov = "COVERAGE=1"
	FileName = "FILENAME=" + file
	BuildOptions = getTestBuildFlag()
	if getCleanFlag():
		BuildOptions = "clean"

	# build and run the test
	make_process = subprocess.Popen(["make", "-s", "-j2", BuildOptions, FileName, ObjectList, Cov], stderr=subprocess.STDOUT)
	if make_process.wait() != 0:
		raise Exception

# removes temporary objects for tests and coverage
def cleanTestFiles():

	#delete gcov files
	sourcepath="./tests/gcov/"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".gcov"):
			os.remove(os.path.join(sourcepath,files))

	#delete object files
	sourcepath="./tests/objs/debug/"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".o"):
			os.remove(os.path.join(sourcepath,files))

	#delete gcno files
	sourcepath="./tests/objs/debug/"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".gcno"):
			os.remove(os.path.join(sourcepath,files))

	#delete gcno files
	sourcepath="./tests/deps/debug/"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".d"):
			os.remove(os.path.join(sourcepath,files))
	#delete executable files
	sourcepath="./tests/bin/debug/"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".test"):
			os.remove(os.path.join(sourcepath,files))


#compiles all the .cpp files in the ./tests directory
def compileTestFiles():
	if getIsArgument( "cov" ):
		cleanTestFiles()
	# move into the test directory
	os.chdir( "./tests" )
	if getIsArgument( "cov" ):
		pool = multiprocessing.Pool(1)
	else:
		pool = multiprocessing.Pool()
	results = []
	# get list of tests and compile/run them in separate processes.
	for root, dirs, files in os.walk("./source/"):
		for file in files:
			if file.endswith('.cpp'):
				fullpath = os.path.join(root, file)
				results.append(pool.apply_async(compileTest, args=(fullpath,)))
	#wait
	pool.close()
	pool.join()

	#delete gcov files
	sourcepath="./"
	source = os.listdir(sourcepath)
	for files in source:
		if files.endswith(".gcov"):
			os.remove(os.path.join(sourcepath,files))

	os.chdir( "../" )

	#check results and exit on an error
	for result in results:
		if not result.successful():
			raise SystemExit


def buildDocs():

	if getCleanFlag():
		#clean docs
		os.chdir( "./docs" )
		if os.path.isdir("./html" ):
			shutil.rmtree("./html")
		os.chdir( "../" )
	else:
		os.chdir( "./doxy" )
		if isNotInstalled( "doxygen" ):
			printHighlight( "Error: doxygen is not installed" )
		else:

			if isNotInstalled( "dot" ):
				printHighlight( "Error: graphviz is not installed" )
			else:
				make_process = subprocess.Popen(["doxygen", "Doxyfile"], stderr=subprocess.STDOUT)
				if make_process.wait() != 0:
					print("Error creating docs")
		os.chdir( "../" )

def CleanTemps():
	if getCleanFlag():
		#clean temp files
		os.chdir( "./objs" )
		if os.path.isdir("./debug" ):
			shutil.rmtree("./debug")
			os.makedirs( "./debug" )
		os.chdir( "../" )
		os.chdir( "./tests" )
		if os.path.isdir("./gcov" ):
			shutil.rmtree("./gcov")
			os.makedirs( "./gcov" )
		os.chdir( "./objs" )
		if os.path.isdir("./debug" ):
			shutil.rmtree("./debug")
			os.makedirs( "./debug" )
		os.chdir( "../../" )

def main():

	#check for and build missing dependencies
	printMinorHeader("Checking Externals")
	buildExternals()
	printHighlight("Finished")

	#create missing directories
	buildDirectories()

	printMajorHeader("Building " + os.path.basename(os.path.dirname(os.path.realpath(__file__))))
	printProjectLineCount()

	# cppcheck
	printMinorHeader( "Performing Code Analysis" )
	codeAnalysis()
	printHighlight( "Finished" )

	# build program
	printMinorHeader( "Building Production code" )
	compileProductionFiles()
	printHighlight( "Finished" )

	# build tests
	printMinorHeader( "Building/Running Tests" )
	compileTestLibs()
	compileTestFiles()
	printHighlight( "Finished" )

	# doxygen
	if getIsArgument("docs"):
		printMinorHeader( "Building Documentation" )
		buildDocs()
		printHighlight( "Finished" )

	# remove temporary files if the clean flag is set
	CleanTemps()

	print("")
	printHighlight( "****Build Complete****" )
	print("")


if __name__ == "__main__":
	main()