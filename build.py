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
sys.dont_write_bytecode = True
import os
sys.path.insert(0,os.path.abspath("./server"))
sys.path.insert(0,os.path.abspath("./tools"))

import build_server
import build_tools

def main():

	os.chdir( "./server" )
	build_server.main()
	os.chdir( "../" )
	os.chdir( "./tools" )
	build_tools.main()
	os.chdir( "../" )

if __name__ == "__main__":
	main()
