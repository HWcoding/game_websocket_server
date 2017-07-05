
RELEASE_LIBS = -lpthread -lstdc++ -lcrypto -lm
DEBUG_LIBS = -rdynamic -lpthread -lstdc++ -lcrypto -ldl
CPP_VERSION = -std=c++14
FILE_LIST = "./source/main.cpp"
COMP = g++
CFLAGS = -DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline

# we need to use absolute path for dependency files to work properly
PROJECT_DIRECTORY := $(abspath ../../)/

# use the full path of the cpp file in object file names to
# prevent multiple files with the same name in different
# folders from overwriting each other in the /objs folder
# we replace all the / with +. to get the file name and then
# append it to the path. +. was chosen because it is unlikely to
# be part of a filename and has no special meaning in make
OBJECT_NAMES = $(subst  /,+., $(basename $(abspath $(FILE_LIST))))

# release
RELEASE_OBJ_DIR = $(PROJECT_DIRECTORY)tests/objs/release/
RELEASE_DEP_DIR = $(PROJECT_DIRECTORY)tests/deps/release/
RELEASE_OBJECTS = $(addprefix $(RELEASE_OBJ_DIR), $(patsubst %,%.o,$(OBJECT_NAMES)))
RELEASE_DEPFILES = $(patsubst %,$(RELEASE_DEP_DIR)%.d,$(basename $(notdir $(RELEASE_OBJECTS))))
RELEASE_LIBRARY_NAME = $(RELEASE_OBJ_DIR)test_lib.a

#debug
DEBUG_OBJ_DIR = $(PROJECT_DIRECTORY)tests/objs/debug/
DEBUG_DEP_DIR = $(PROJECT_DIRECTORY)tests/deps/debug/
DEBUG_OBJECTS = $(addprefix $(DEBUG_OBJ_DIR), $(patsubst %,%.o,$(OBJECT_NAMES)))
DEBUG_DEPFILES = $(patsubst %,$(DEBUG_DEP_DIR)%.d,$(basename $(notdir $(DEBUG_OBJECTS))))
DEBUG_LIBRARY_NAME = $(DEBUG_OBJ_DIR)test_lib.a


# check to see if this is a debug build
ifneq (,$(findstring -DDEBUG,$(CFLAGS)))
	LIBRARIES = $(DEBUG_LIBS)
	OBJ_DIR = $(DEBUG_OBJ_DIR)
	DEP_DIR = $(DEBUG_DEP_DIR)
	OBJECTS = $(DEBUG_OBJECTS)
	DEPFILES = $(DEBUG_DEPFILES)
	LIBRARY_NAME = $(DEBUG_LIBRARY_NAME)
else
	LIBRARIES = $(RELEASE_LIBS)
	OBJ_DIR = $(RELEASE_OBJ_DIR)
	DEP_DIR = $(RELEASE_DEP_DIR)
	OBJECTS = $(RELEASE_OBJECTS)
	DEPFILES = $(RELEASE_DEPFILES)
	LIBRARY_NAME = $(RELEASE_LIBRARY_NAME)
endif

DEP_TEMP = $(DEP_DIR)temp/
HEAD_PATH = $(PROJECT_DIRECTORY)
TEST_FRAMEWORK_HEADER = $(PROJECT_DIRECTORY)external/google_test/include/

#compiler flags for generating dependency file
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_TEMP)$*.Td

# we write our dependencies to a temp folder and then move them into
# the deps folder after they compile successfully. This is done to prevent
# the dependency files from getting corrupted in a failed compile.

# command to move dependencies from the temp folder into the deps folder
MOV_DEPS = mv -f $(DEP_TEMP)$*.Td $(DEP_DIR)$*.d

CXX = $(COMP) -I $(HEAD_PATH) -isystem $(TEST_FRAMEWORK_HEADER) $(CFLAGS) $(DEPFLAGS) $(CPP_VERSION)

all: $(LIBRARY_NAME)

$(LIBRARY_NAME): $(OBJECTS)
	gcc-ar rcs $@ $^

# in order for this rule to work filenames cannot contain the
# string -!- or any of the special symbols make uses such as @ or $
# In order to get the .cpp filename we replace all the -!- with /
# This will give us the full path of the file.
.SECONDEXPANSION:
$(OBJ_DIR)%.o: $$(subst  +.,/, %.cpp) $(DEP_DIR)%.d
	$(CXX) -c -o $@ $<
	$(MOV_DEPS)

.PRECIOUS: $(DEP_DIR)%.d $(CPP_DIR)%.h.gch

$(DEP_DIR)%.d: ;

.PHONY: clean
clean:
	rm -f $(RELEASE_OBJECTS)
	rm -f $(RELEASE_DEPFILES)

	rm -f $(DEBUG_OBJECTS)
	rm -f $(DEBUG_DEPFILES)

	rm -f $(DEBUG_LIBRARY_NAME)
	rm -f $(RELEASE_LIBRARY_NAME)

-include $(DEPFILES)
