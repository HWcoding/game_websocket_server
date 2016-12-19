Requires Doxygen for documentation generation and cppcheck for code analysis.

Google test and Google mock should be compiled into static libraries and placed in:

/tests/gmock_main.a
/tests/gtest_main.a

to build the tests.

The TEST_FRAMEWORK_HEADER and MOCK_FRAMEWORK_HEADER variables in the /tests/makefile need to point to the /googlemock/include and /googletest/include directories.