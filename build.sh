#!/bin/bash

# Options:
#	debug
#		builds a debug build (this is the default)
#	release
#		builds a release build
#	profile
#		builds a profile build for use with a profiler
#	clean
#		runs 'make clean' to remove temparary objs
#	weak
#		uses weaker warnings while building


# build flags
StrongWarnings=" -Winvalid-pch -fmax-errors=5 -Wall -Wextra -Wformat=2 -Wformat-signedness\
 -Wmissing-include-dirs -Wswitch-default -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wconversion\
 -Wuseless-cast -Wsign-conversion -Wfloat-conversion -Wlogical-op -Wmissing-declarations -pedantic\
 -ansi -Weffc++ -Werror"

WeakWarnings=" -Wall -Wextra -pedantic -ansi -Weffc++"
TestWarnings=" -Wall"
DebugBuild="CFLAGS=-DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline"
ReleaseBuild="CFLAGS=-DNDEBUG -Ofast -march=native"
ProfileBuild="CFLAGS=-DDEBUG -Ofast -g3 -fno-omit-frame-pointer -march=native"

# convert arguments to lower case
FirstArg=$( echo "${1}" | tr '[:upper:]' '[:lower:]')
SecondArg=$( echo "${2}" | tr '[:upper:]' '[:lower:]')
ThirdArg=$( echo "${3}" | tr '[:upper:]' '[:lower:]')
FourthArg=$( echo "${4}" | tr '[:upper:]' '[:lower:]')


########################################################################
# Color printing functions

# checks to see if 256 color terminal is supported
ColorSupported=""
function checkColorSupport() {
	if [ "${ColorSupported}" = "" ]; then
		local colorCount=$(tput colors)
		if [ "${colorCount}" = "256" ]; then
			ColorSupported="true"
		else
			ColorSupported="false"
		fi
	fi
	echo "${ColorSupported}"
}

# echos input inside a blue gradiant bar
function printMajorHeaderColor() {
	echo -en "\033[48;5;17m "
	echo -en "\033[48;5;18m "
	echo -en "\033[48;5;19m "
	echo -en "\033[48;5;20m "
	echo -en "\033[1;37;48;5;21m                   "
	echo -en "${1}"
	echo -en "\033[48;5;21m                   "
	echo -en "\033[48;5;20m "
	echo -en "\033[48;5;19m "
	echo -en "\033[48;5;18m "
	echo -en "\033[48;5;17m "
	echo -e "\033[0m"
}

# echos input between 2 rows of asterisks
function printMajorHeaderText() {
	local borderString=""
	# the width of the header in characters
	local headerWidth=75
	# calculate the number of spaces we need to
	# place before the input to center it
	local spaceCount=$(((headerWidth-${#1})/2))
	local count=0
	while [ $count -lt $headerWidth ]; do
		borderString+="*"
		count=$((count+1))
	done
	local centerSpaces=""
	count=0
	while [ $count -lt $spaceCount ]; do
		centerSpaces+=" "
		count=$((count+1))
	done
	local centerString=""
	centerString+="${centerSpaces}"
	centerString+="${1}"
	echo "${borderString}"
	echo "${centerString}"
	echo "${borderString}"
}

# echos a large header containing the input
function printMajorHeader() {
	local supported="$(checkColorSupport)"
	if [ "${supported}" = "true" ]; then
		echo -e "\n$(printMajorHeaderColor "${1}")\n"
	else
		echo -e "\n$(printMajorHeaderText "${1}")\n"
	fi
}


# echos the input with a green background
function printMinorHeaderColor() {
	echo -en "\033[1;37;48;5;17m   "
	echo -en "${1}"
	echo -en "   "
	echo -e "\033[0m"
}

# echos input in a row of asterisks
function printMinorHeaderText() {
	# the width of the header in characters
	local headerWidth=55
	# calculate the number of asterisks we need to
	# place before the input to center it
	local asteriskCount=$(((headerWidth-${#1}-2)/2))
	local asterisks=""
	local count=0
	while [ $count -lt $asteriskCount ]; do
		asterisks+="*"
		count=$((count+1))
	done
	local centerString="${asterisks}"
	centerString+="${1}"
	centerString+="${asterisks}"
	echo "${centerString}"
}

# echos a small header containing the input
function printMinorHeader() {
	local supported="$(checkColorSupport)"
	if [ "${supported}" = "true" ]; then
		echo -e "\n$(printMinorHeaderColor "${1}")\n"
	else
		echo -e "\n$(printMinorHeaderText "${1}")\n"

	fi
}


# echos input with green formating
function printHighlightColor() {
	echo -en "\033[92m"
	echo -en  "${1}"
	echo -e "\033[0m"
}

# echos a highlighted version of input
function printHighlight() {
	local supported="$(checkColorSupport)"
	if [ "${supported}" = "true" ]; then
		echo -e "$(printHighlightColor "${1}")"
	else
		echo "[${1}]"
	fi
}


##################################################################



# checks if input is a command line argument
function getIsArgument() {
	if [ "${FirstArg}" == "${1}" ] || [ "${SecondArg}" == "${1}" ] ||
		[ "${ThirdArg}" == "${1}" ] || [ "${FourthArg}" == "${1}" ]
		then
		echo "true"
	else
		echo "false"
	fi
}

# returns true if temparary files should be deleted. Set by passing clean as a command argument
CleanFlag=""
function getCleanFlag() {
	if [ "${CleanFlag}" = "" ]; then
		if [ $(getIsArgument clean) == "true" ]; then
			CleanFlag="true"
		else
			CleanFlag="false"
		fi
	fi
	echo "${CleanFlag}"
}

# returns a string containing the build flags to use with makefile
BuildFlag=""
function getBuildFlag() {
	if [ "${BuildFlag}" = "" ]; then
		if [ $(getIsArgument debug) == "true" ]; then
			BuildFlag="${DebugBuild}"
		elif [ $(getIsArgument profile) == "true" ]; then
			BuildFlag="${ProfileBuild}"
		elif [ $(getIsArgument release) == "true" ]; then
			BuildFlag="${ReleaseBuild}"
		else
			BuildFlag="${DebugBuild}"
		fi
	fi
	echo "${BuildFlag}"
}

# returns a string containing the build flags to use with the production makefile including warnings
ProductionBuildFlag=""
function getProductionBuildFlag() {
	if [ "${ProductionBuildFlag}" = "" ]; then
		ProductionBuildFlag="$(getBuildFlag)"
		if [ $(getIsArgument weak) == "true" ]; then
			ProductionBuildFlag+="${WeakWarnings}"
		else
			ProductionBuildFlag+="${StrongWarnings}"
		fi
	fi
	echo "${ProductionBuildFlag}"
}

# returns a string containing the build flags to use with the test makefile including warnings
TestBuildFlag=""
function getTestBuildFlag() {
	if [ "${TestBuildFlag}" = "" ]; then
		TestBuildFlag="$(getBuildFlag)"
		TestBuildFlag+="${TestWarnings}"
	fi
	echo "${TestBuildFlag}"
}

# returns a string describing the build type.
BuildType=""
function getBuildType() {
	if [ "${BuildType}" = "" ]; then
		if [ $(getIsArgument profile) == "true" ]; then
			BuildType="profile"
		elif [ $(getIsArgument release) == "true" ]; then
			BuildType="release"
		else
			BuildType="debug"
		fi
	fi
	echo "${BuildType}"
}

# checks to see if previous build used the same build flags
CompatableBuild=""
function checkBuildCompatabilty() {
	if [ "${CompatableBuild}" = "" ]; then
		local previousBuildType=""
		if [ -f "./lastBuildType.txt" ]; then
			while read -r line;	do
				previousBuildType="${line}"
			done < "./lastBuildType.txt"
		fi
		if [ "${previousBuildType}" = "$(getBuildType)" ]; then
			CompatableBuild="true"
		else
			CompatableBuild="false"
		fi
	fi
	echo "${CompatableBuild}"
}

# writes the current build type to file.
function writeBuildType() {
	echo "$(getBuildType)" > "./lastBuildType.txt"
}

# takes a test .cpp filename as an argument and returns dependencies listed in it
function getTestDependencies() {
	# get a list of dependencies from the cpp file
	local Dependencies=()
	local DependentcyString=""
	while read -r line;	do
		# check to see if the line begins with our macro
		if [[ "${line}" == "#define TEST_FILE_LINK_DEPENDENCIES"* ]]; then
        	readingDependency="yes"
		fi
		# If variable is yes, append
		if [[ $readingDependency == "yes" ]]; then
			# if line does not end in \ we have finished
			local lastChar="${line: -1}"
			if [[ "${lastChar}" != "\\" ]]; then
				readingDependency="no"
			else
				# remove ending \
				line="${line%?}"
			fi
			# append line
			DependentcyString="${DependentcyString}${line}"
		fi
	done < "${1}"

	# remove the beginning quote and everything before it
	local tmp="${DependentcyString#*\"}"
	# remove the ending quote and everything after it
	DependentcyString="${tmp%\"*}"
	# split the line at commas and store in array
	IFS=',' read -r -a array <<< "$DependentcyString"
	# loop through array and add each element to Dependencies
	for element in "${array[@]}"; do
		Dependencies+=" ""${element}"
	done

	echo "${Dependencies}"
}

# takes a test .cpp filename as argument and complies it
function compileTest() {
	local ObjectList="OBJECT_LIST=""$(getTestDependencies "${1}")"
	local FileName="FILENAME=""${1}"
	local BuildOptions="$(getTestBuildFlag)"
	if [ "$(getCleanFlag)" == "true" ]; then
		BuildOptions="clean"
	fi
	# build and run the test
	make -s -j2 "${BuildOptions}" "${ObjectList}" "${FileName}"
}

#compiles all the .cpp files in the ./tests directory
function compileTestFiles() {


	# move into the test directory
	cd ./tests
	# get list of tests and compile/run them in separate processes.
	while read -r -d $'\0' files; do
		{ compileTest "${files}"; } &
	done < <(find ./source -type f -name '*.cpp' -print0)
	wait
	cd ../
}

# compiles all the .cpp files in the ./source directory
function compileProductionFiles() {
	# move to source directory
	cd ./source
	# get list of cpp files to compile and store in FileList.
	local Files=()
	while read -d $'\0' file; do
		Files+=" ""${file}"
	done < <(find ./ -type f -name '*.cpp' -print0)
	local FileList="FILE_LIST=""${Files}"
	local BuildOptions="$(getProductionBuildFlag)"
	if [ "$(getCleanFlag)" == "true" ]; then
		BuildOptions="clean"
	fi
	# feed the list to make and build program
	make -s -j8 "${BuildOptions}" "${FileList}"
	cd ../
}

function compileTestLibs() {
	# move to source directory
	cd ./tests/test_lib
	# get list of cpp files to compile and store in FileList.
	local Files=()
	while read -d $'\0' file; do
		Files+=" ""${file}"
	done < <(find ./ -type f -name '*.cpp' -print0)
	local FileList="FILE_LIST=""${Files}"
	local BuildOptions="$(getProductionBuildFlag)"
	if [ "$(getCleanFlag)" == "true" ]; then
		BuildOptions="clean"
	fi
	# feed the list to make and build program
	make -s -j8 "${BuildOptions}" "${FileList}"
	cd ../../
}

# checks to see if this build uses the same build type as the last
# if not, it cleans the temp files and starts the build from scratch
function makeBuildCompatable() {
	if [ "$(getCleanFlag)" = "false" ]; then
		if [ "$(checkBuildCompatabilty)" = "false" ]; then
			CleanFlag="true"
			compileProductionFiles
			compileTestFiles
			CleanFlag="false"
		fi
		writeBuildType
	fi
}

function printProjectLineCount() {
	# get line count
	cd ./source
	ProductionLines="$(find . \( -name '*.h' -o -name '*.cpp' \) -exec cat {} + | wc -l)"
	cd ../tests
	TestLines="$(find . \( -name '*.h' -o -name '*.cpp' \) -exec cat {} + | wc -l)"
	cd ../
	echo -e "$(printHighlight "Project contains:") ${ProductionLines} lines of production code"
	echo -e "$(printHighlight "Project contains:") ${TestLines} lines of test code"
	echo -e "$(printHighlight "Total is:")         $((TestLines + ProductionLines)) lines of code"
}

function buildDocs() {
	if [ "$(getCleanFlag)" == "false" ]; then
		doxygen Doxyfile
	else

		#clean docs
		cd ./docs/html
		rm -rf ./*
		cd ../../
	fi
}


function codeAnalysis() {
	if [ "$(getCleanFlag)" == "false" ]; then
		./external/cppcheck/cppcheck "--quiet" "-I./" "--enable=warning,performance,information" "-j8" "--cppcheck-build-dir=./analysis" "./source"
	else
		cd ./analysis
		rm ./*
		cd ../
	fi

}





#fetch and build POCO network library
function buildPoco() {
	if [ ! -f "poco_built" ]; then
		printMinorHeader "Building portable components (Poco) network library"
		printHighlight "Installing openssl, libssl-dev, libpq-dev, unixodbc, and unixodbc-dev"
		sudo apt-get install openssl libssl-dev libpq-dev unixodbc unixodbc-dev

		if [ -d "poco" ]; then
			rm -rf ./poco
		fi
		mkdir poco
		cd ./poco
		mkdir lib
		mkdir include
		cd ./include
		mkdir Poco
		cd ../../
		if [ ! -f "poco_downloaded" ]; then
			if [ -d "poco_source" ]; then
				rm -rf ./poco_source
			fi
			echo ""
			printHighlight "Cloning Poco c++ network library from github at:\nhttps://github.com/pocoproject/poco.git"
				git clone https://github.com/pocoproject/poco.git poco_source
			touch poco_downloaded
		fi

		cd ./poco_source
		echo""
		printHighlight "Compiling Poco"
		./configure --everything --static --no-tests --poquito --no-samples --typical --omit=Data/MySQL --cflags="-O3 -march=native" &>build.log
		make -j8 -s &>build.log

		POCOLIB_SOURCE="./lib/Linux/x86_64"
		POCOLIB_DEST="../poco/lib/"

		cp ${POCOLIB_SOURCE}/libPocoUtil.a ${POCOLIB_DEST}/libPocoUtil.a
		cp ${POCOLIB_SOURCE}/libPocoCrypto.a ${POCOLIB_DEST}/libPocoCrypto.a
		cp ${POCOLIB_SOURCE}/libPocoJSON.a ${POCOLIB_DEST}/libPocoJSON.a
		cp ${POCOLIB_SOURCE}/libPocoNet.a ${POCOLIB_DEST}/libPocoNet.a
		cp ${POCOLIB_SOURCE}/libPocoNetSSL.a ${POCOLIB_DEST}/libPocoNetSSL.a
		cp ${POCOLIB_SOURCE}/libPocoXML.a ${POCOLIB_DEST}/libPocoXML.a
		cp ${POCOLIB_SOURCE}/libPocoFoundation.a ${POCOLIB_DEST}/libPocoFoundation.a
		cp ${POCOLIB_SOURCE}/libPocoCppParser.a ${POCOLIB_DEST}/libPocoCppParser.a
		cp ${POCOLIB_SOURCE}/libPocoData.a ${POCOLIB_DEST}/libPocoData.a
		cp ${POCOLIB_SOURCE}/libPocoDataODBC.a ${POCOLIB_DEST}/libPocoDataODBC.a
		cp ${POCOLIB_SOURCE}/libPocoDataPostgreSQL.a ${POCOLIB_DEST}/libPocoDataPostgreSQL.a
		cp ${POCOLIB_SOURCE}/libPocoDataSQLite.a ${POCOLIB_DEST}/libPocoDataSQLite.a
		cp ${POCOLIB_SOURCE}/libPocoMongoDB.a ${POCOLIB_DEST}/libPocoMongoDB.a
		cp ${POCOLIB_SOURCE}/libPocoPDF.a ${POCOLIB_DEST}/libPocoPDF.a
		cp ${POCOLIB_SOURCE}/libPocoRedis.a ${POCOLIB_DEST}/libPocoRedis.a
		cp ${POCOLIB_SOURCE}/libPocoZip.a ${POCOLIB_DEST}/libPocoZip.a

		cd ${POCOLIB_DEST}

		# create thin archive for linking
		while read -d $'\0' file; do
			ar -rT poco.a "${file}" &>build.log
		done < <(find ./ -type f -name '*.a' -print0)

		cd ../../poco_source

		cp ./CONTRIBUTORS ../poco/CONTRIBUTORS
		cp ./README ../poco/poco_README
		cp ./README.md ../poco/poco_README.md
		cp ./LICENSE ../poco/poco_LICENCE

		cd ../

		POCOHEAD_SOURCE="./poco_source"
		POCOHEAD_DEST="./poco/include"

		cd ./poco/include
		mkdir openssl
		cd ./Poco
		mkdir CppParser
		mkdir Crypto
		mkdir Data
		mkdir JSON
		mkdir MongoDB
		mkdir Net
		mkdir PDF
		mkdir Redis
		mkdir SevenZip
		mkdir Util
		mkdir XML
		mkdir Zip
		cd ../../../

		cp -r ${POCOHEAD_SOURCE}/Foundation/include/Poco/* ${POCOHEAD_DEST}/Poco
		cp -r ${POCOHEAD_SOURCE}/CppParser/include/Poco/CppParser/* ${POCOHEAD_DEST}/Poco/CppParser
		cp -r ${POCOHEAD_SOURCE}/Crypto/include/Poco/Crypto/* ${POCOHEAD_DEST}/Poco/Crypto
		cp -r ${POCOHEAD_SOURCE}/Data/include/Poco/Data/* ${POCOHEAD_DEST}/Poco/Data
		cp -r ${POCOHEAD_SOURCE}/JSON/include/Poco/JSON/* ${POCOHEAD_DEST}/Poco/JSON
		cp -r ${POCOHEAD_SOURCE}/MongoDB/include/Poco/MongoDB/* ${POCOHEAD_DEST}/Poco/MongoDB
		cp -r ${POCOHEAD_SOURCE}/Net/include/Poco/Net/* ${POCOHEAD_DEST}/Poco/Net
		cp -r ${POCOHEAD_SOURCE}/NetSSL_OpenSSL/include/Poco/Net/* ${POCOHEAD_DEST}/Poco/Net
		cp -r ${POCOHEAD_SOURCE}/openssl/include/openssl/* ${POCOHEAD_DEST}/openssl
		cp -r ${POCOHEAD_SOURCE}/PDF/include/Poco/PDF/* ${POCOHEAD_DEST}/Poco/PDF
		cp -r ${POCOHEAD_SOURCE}/Redis/include/Poco/Redis/* ${POCOHEAD_DEST}/Poco/Redis
		cp -r ${POCOHEAD_SOURCE}/SevenZip/include/Poco/SevenZip/* ${POCOHEAD_DEST}/Poco/SevenZip
		cp -r ${POCOHEAD_SOURCE}/Util/include/Poco/Util/* ${POCOHEAD_DEST}/Poco/Util
		cp -r ${POCOHEAD_SOURCE}/XML/include/Poco/XML/* ${POCOHEAD_DEST}/Poco/XML
		cp -r ${POCOHEAD_SOURCE}/Zip/include/Poco/Zip/* ${POCOHEAD_DEST}/Poco/Zip

		rm poco_downloaded
		rm -rf ./poco_source
		touch poco_built

		printHighlight "Finished"
	fi
}



#fetch and build google test framework
function buildGoogleTest(){
	if [ ! -f "google_test_built" ]; then
		printMinorHeader "Building Google Test"
		if [ -d "google_test" ]; then
			rm -rf ./google_test
		fi
		mkdir google_test
		cd ./google_test
		mkdir include
		cd ../
		if [ ! -f "google_test_and_mock_downloaded" ]; then
			if [ -d "google_test_and_mock" ]; then
				rm -rf ./google_test_and_mock
			fi
			echo ""
			printHighlight "Cloning googletest from github at:\nhttps://github.com/google/googletest.git"
				git clone https://github.com/google/googletest.git google_test_and_mock
			touch google_test_and_mock_downloaded
		fi
		cd ./google_test_and_mock/

		GTEST_DIR="./googletest"
		GMOCK_DIR="./googlemock"

		echo ""
		printHighlight "Compiling Google Test"
		g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
			-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
			-pthread -Ofast -march=native -c ${GTEST_DIR}/src/gtest-all.cc
		g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
			-isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
			-pthread -Ofast -march=native -c ${GMOCK_DIR}/src/gmock-all.cc
		ar -rv ../google_test/google_test.a gtest-all.o gmock-all.o

		cd ../
		GLIB_SOURCE="./google_test_and_mock"
		GLIB_DEST="./google_test"

		cp -r ${GLIB_SOURCE}/googletest/include/* ${GLIB_DEST}/include
		cp ${GLIB_SOURCE}/README.md ${GLIB_DEST}/googletest_README.md
		cp ${GLIB_SOURCE}/googletest/LICENSE ${GLIB_DEST}/googletest_LICENSE
		cp ${GLIB_SOURCE}/googlemock/LICENSE ${GLIB_DEST}/googlemock_LICENSE
		cp ${GLIB_SOURCE}/googlemock/LICENSE ${GLIB_DEST}/googlemock_LICENSE
		cp ${GLIB_SOURCE}/googletest/CONTRIBUTORS ${GLIB_DEST}/googletest_CONTRIBUTORS
		cp ${GLIB_SOURCE}/googlemock/CONTRIBUTORS ${GLIB_DEST}/googlemock_CONTRIBUTORS

		rm google_test_and_mock_downloaded
		rm -rf ./google_test_and_mock
		touch google_test_built
		printHighlight "Finished"
	fi
}

#fetch and build Cppcheck code analysis tool
function buildCppcheck(){
	if [ ! -f "cppcheck_built" ]; then
		printMinorHeader "Building Cppcheck"
		if [ -d "cppcheck" ]; then
			rm -rf ./cppcheck
		fi
		mkdir cppcheck

		if [ ! -f "cppcheck_downloaded" ]; then
			if [ -d "cppcheck_source" ]; then
				rm -rf ./cppcheck_source
			fi
			echo ""
			printHighlight "Cloning Cppcheck code analysis tool from github at:\nhttps://github.com/danmar/cppcheck.git"
			git clone https://github.com/danmar/cppcheck.git cppcheck_source
			touch cppcheck_downloaded
		fi
		cd ./cppcheck_source

		echo ""
		printHighlight "Compiling Cppcheck"
		g++ -Ofast -march=native -DNDEBUG -o cppcheck -std=c++0x \
			-include lib/cxx11emu.h -Iexternals/simplecpp -Iexternals/tinyxml \
			-Ilib cli/*.cpp lib/*.cpp externals/tinyxml/*.cpp \
			externals/simplecpp/*.cpp
		cd ../
		cp ./cppcheck_source/cppcheck ./cppcheck/cppcheck
		cp ./cppcheck_source/AUTHORS ./cppcheck/cppcheck_AUTHORS
		cp ./cppcheck_source/COPYING ./cppcheck/cppcheck_LICENSE
		cp ./cppcheck_source/readme.md ./cppcheck/cppcheck_readme.md
		cp ./cppcheck_source/readme.txt ./cppcheck/cppcheck_readme.txt
		cp ./cppcheck_source/cfg/std.cfg ./cppcheck/std.cfg

		rm cppcheck_downloaded
		rm -rf ./cppcheck_source
		touch cppcheck_built

		printHighlight "Finished"
	fi
}

function installDoxy(){
	if [ ! -f "doxy_installed" ]; then
		sudo apt-get install doxygen
		touch doxy_installed
	fi
}

# build external dependencies
function buildExternals(){
	if [ ! -d "external" ]; then
		mkdir external
	fi
	cd ./external
	buildPoco
	buildGoogleTest
	buildCppcheck
	installDoxy
	cd ../
}





function main() {
	# set the script to stop on error
	set -e
	printMajorHeader "Building "${PWD##*/}""
	printProjectLineCount

	#check for and build missing dependencies
	printMinorHeader "Checking Externals"
	buildExternals
	printHighlight "Finished"

	makeBuildCompatable
	# cppcheck
	set +e
	printMinorHeader "Performing Code Analysis"
	codeAnalysis
	printHighlight "Finished"
	set -e
	# build program
	printMinorHeader "Building Production code"
	compileProductionFiles
	printHighlight "Finished"
	set +e
	# build tests
	printMinorHeader "Building/Running Tests"
	compileTestLibs
	compileTestFiles
	printHighlight "Finished"
	# doxygen
	printMinorHeader "Building Documentation"
	buildDocs
	printHighlight "Finished"
	echo ""
	printHighlight "****Build Complete****"
	echo ""
}

main
