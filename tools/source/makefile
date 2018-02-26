
EXECUTABLE_NAME = program
RELEASE_LIBS = -lpthread -lstdc++ -lcrypto -lz
DEBUG_LIBS = -rdynamic -lpthread -lstdc++ -lcrypto -ldl -lz
CPP_VERSION = -std=c++14
FILE_LIST = "./source/main.cpp"
COMP = g++
CFLAGS = -DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline

# we need to use absolute path for dependency files to work properly
PROJECT_DIRECTORY := $(abspath ../)/

# use the full path of the cpp file in object file names to
# prevent multiple files with the same name in different
# folders from overwriting each other in the /objs folder
# we replace all the / with +. to get the file name and then
# append it to the path. +. was chosen because it is unlikely to
# be part of a filename and has no special meaning in make
OBJECT_NAMES = $(subst  /,+., $(basename $(abspath $(FILE_LIST))))

# release
RELEASE_OBJ_DIR = $(PROJECT_DIRECTORY)objs/release/
RELEASE_DEP_DIR = $(PROJECT_DIRECTORY)deps/release/
RELEASE_OBJECTS = $(addprefix $(RELEASE_OBJ_DIR), $(patsubst %,%.o,$(OBJECT_NAMES)))
RELEASE_DEPFILES = $(patsubst %,$(RELEASE_DEP_DIR)%.d,$(basename $(notdir $(RELEASE_OBJECTS))))
RELEASE_EXECUTABLE = $(PROJECT_DIRECTORY)$(EXECUTABLE_NAME)_Release

#debug
DEBUG_OBJ_DIR = $(PROJECT_DIRECTORY)objs/debug/
DEBUG_DEP_DIR = $(PROJECT_DIRECTORY)deps/debug/
DEBUG_OBJECTS = $(addprefix $(DEBUG_OBJ_DIR), $(patsubst %,%.o,$(OBJECT_NAMES)))
DEBUG_DEPFILES = $(patsubst %,$(DEBUG_DEP_DIR)%.d,$(basename $(notdir $(DEBUG_OBJECTS))))
DEBUG_EXECUTABLE = $(PROJECT_DIRECTORY)$(EXECUTABLE_NAME)_Debug
DEBUG_COVERAGE = $(DEBUG_OBJ_DIR)coverage/


# check to see if this is a debug build
ifneq (,$(findstring -DDEBUG,$(CFLAGS)))
	LIBRARIES = $(DEBUG_LIBS)
	OBJ_DIR = $(DEBUG_OBJ_DIR)
	DEP_DIR = $(DEBUG_DEP_DIR)
	OBJECTS = $(DEBUG_OBJECTS)
	DEPFILES = $(DEBUG_DEPFILES)
	EXECUTABLE = $(DEBUG_EXECUTABLE)
else
	LIBRARIES = $(RELEASE_LIBS)
	OBJ_DIR = $(RELEASE_OBJ_DIR)
	DEP_DIR = $(RELEASE_DEP_DIR)
	OBJECTS = $(RELEASE_OBJECTS)
	DEPFILES = $(RELEASE_DEPFILES)
	EXECUTABLE = $(RELEASE_EXECUTABLE)
endif

DEP_TEMP = $(DEP_DIR)temp/
HEAD_PATH = $(PROJECT_DIRECTORY)


#compiler flags for generating dependency file
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_TEMP)$*.Td

# we write our dependencies to a temp folder and then move them into
# the deps folder after they compile successfully. This is done to prevent
# the dependency files from getting corrupted in a failed compile.

# command to move dependencies from the temp folder into the deps folder
MOV_DEPS = mv -f $(DEP_TEMP)$*.Td $(DEP_DIR)$*.d
MOV_COV = mv -f $(OBJ_DIR)$*.gcno $(OBJ_DIR)coverage/$*_test/$*.gcno

CXX = $(COMP) -I $(HEAD_PATH) $(CFLAGS) $(DEPFLAGS) $(CPP_VERSION)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBRARIES)

# in order for this rule to work filenames cannot contain the
# string +. or any of the special symbols make uses such as @ or $
# In order to get the .cpp filename we replace all the +. with /
# This will give us the full path of the file.

$(OBJ_DIR)%.o: export GCOV_PREFIX = $(addprefix $(DEBUG_COVERAGE), $(basename $(notdir $(%.cpp)))/)
$(OBJ_DIR)%.o: export GCOV_PREFIX_STRIP = 6
.SECONDEXPANSION:
$(OBJ_DIR)%.o: $$(subst  +.,/, %.cpp) $(DEP_DIR)%.d
	$(CXX) -c -o $@ $<
	$(MOV_DEPS)
ifneq (,$(findstring -DDEBUG,$(CFLAGS)))
	mkdir -p $(OBJ_DIR)coverage/$(basename $(notdir $<))_test
	cp -f $(OBJ_DIR)$*.gcno $(OBJ_DIR)coverage/$(basename $(notdir $<))_test/$*.gcno
endif

.PRECIOUS: $(DEP_DIR)%.d $(CPP_DIR)%.h.gch

$(DEP_DIR)%.d: ;

.PHONY: clean
clean:
	rm -f $(RELEASE_OBJECTS)
	rm -f $(RELEASE_DEPFILES)

	rm -f $(DEBUG_OBJECTS)
	rm -f $(DEBUG_DEPFILES)

	rm -f $(DEBUG_EXECUTABLE)
	rm -f $(RELEASE_EXECUTABLE)

-include $(DEPFILES)
