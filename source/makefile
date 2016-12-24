#absolute paths are necessary for dependency files to work properly
EXECUTABLE_NAME = program



# we need to use absolute path for dependency files to work properly
PROJECT_DIRECTORY = $(abspath ../)/

COMP = g++
CFLAGS = -DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline
LINK_TYPE = lib

EXECUTABLE := $(PROJECT_DIRECTORY)$(EXECUTABLE_NAME)
OBJ_DIR := $(PROJECT_DIRECTORY)objs/
DEP_DIR := $(PROJECT_DIRECTORY)deps/
DEP_TEMP := $(DEP_DIR)temp/
HEAD_PATH := $(PROJECT_DIRECTORY)
POCO_HEAD_PATH := $(PROJECT_DIRECTORY)external/poco/include/
POCO_LIB_PATH := $(PROJECT_DIRECTORY)external/poco/$(LINK_TYPE)/

LIBRARIES =  -lpthread -lstdc++ -lm -lcrypto -lssl -ldl

POCO_LIBS = $(POCO_LIB_PATH)poco.a

# use the full path of the cpp file in object file names to
# prevent multiple files with the same name in different
# folders from overwriting each other in the /objs folder
# we replace all the / with -!- to get the file name and then
# append it to the path. -!- was chosen because it is unlikely to
# be part of a filename and has no special meaning in make
OBJECT_NAMES = $(subst  /,-!-, $(basename $(abspath $(FILE_LIST))))
OBJECTS = $(addprefix $(OBJ_DIR), $(patsubst %,%.o,$(OBJECT_NAMES)))

#compiler flags for generating dependency file
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_TEMP)$*.Td

# we write our dependencies to a temp folder and then move them into
# the deps folder after they compile successfully. This is done to prevent
# the dependency files from getting corrupted in a failed compile.

# command to move dependencies from the temp folder into the deps folder
MOV_DEPS = mv -f $(DEP_TEMP)$*.Td $(DEP_DIR)$*.d

CXX = $(COMP) -I $(HEAD_PATH) -isystem $(POCO_HEAD_PATH) $(CFLAGS) $(DEPFLAGS) -std=c++14

DEPFILES = $(patsubst %,$(DEP_DIR)%.d,$(basename $(notdir $(OBJECTS))))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(POCO_LIBS)
	$(CXX) -o $@ $^ $(LIBRARIES)

# in order for this rule to work filenames cannot contain the
# string -!- or any of the special symbols make uses such as @ or $
# In order to get the .cpp filename we replace all the -!- with /
# This will give us the full path of the file.
.SECONDEXPANSION:
$(OBJ_DIR)%.o: $$(subst  -!-,/, %.cpp) $(DEP_DIR)%.d
	$(CXX) -c -o $@ $<
	$(MOV_DEPS)

.PRECIOUS: $(DEP_DIR)%.d $(CPP_DIR)%.h.gch

$(DEP_DIR)%.d: ;

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(DEPFILES)
	rm -f $(EXECUTABLE)

-include $(DEPFILES)