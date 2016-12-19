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
		./cppcheck/cppcheck "--quiet" "-I./" "--enable=warning,performance,information" "-j8" "--cppcheck-build-dir=./analysis" "./source"
	else
		cd ./analysis
		rm ./*
		cd ../
	fi

}

function main() {
	# set the script to stop on error
	set -e
	printMajorHeader "Building "${PWD##*/}""
	printProjectLineCount
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
