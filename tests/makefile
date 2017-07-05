#absolute paths are necessary for dependency files to work properly
#our test executable will have the same name as the cpp file containing the tests
EXECUTABLE_NAME = $(basename $(notdir $(FILENAME)))
LIBRARIES = -lpthread -lstdc++ -lcrypto -ldl -lm

PROJECT_DIRECTORY = $(abspath ../)/

COMP = g++
CFLAGS = -DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline








CPP_FILE_NAME = $(abspath $(FILENAME))
TEST_OBJECTS_NAME = $(subst  /,+., $(basename $(CPP_FILE_NAME)))



#release
RELEASE_OBJ_DIR = $(PROJECT_DIRECTORY)tests/objs/release/
RELEASE_DEP_DIR = $(PROJECT_DIRECTORY)tests/deps/release/
RELEASE_EXECUTABLE = $(PROJECT_DIRECTORY)tests/bin/release/$(EXECUTABLE_NAME).test
RELEASE_TEST_OBJECTS = $(addsuffix .o, $(addprefix $(RELEASE_OBJ_DIR), $(TEST_OBJECTS_NAME)))
RELEASE_DEPFILES := $(patsubst %,$(RELEASE_DEP_DIR)%.d,$(basename $(notdir $(RELEASE_TEST_OBJECTS))))

#debug
DEBUG_OBJ_DIR = $(PROJECT_DIRECTORY)tests/objs/debug/
DEBUG_DEP_DIR = $(PROJECT_DIRECTORY)tests/deps/debug/
DEBUG_EXECUTABLE = $(PROJECT_DIRECTORY)tests/bin/debug/$(EXECUTABLE_NAME).test
DEBUG_TEST_OBJECTS = $(addsuffix .o, $(addprefix $(DEBUG_OBJ_DIR), $(TEST_OBJECTS_NAME)))
DEBUG_DEPFILES := $(patsubst %,$(DEBUG_DEP_DIR)%.d,$(basename $(notdir $(DEBUG_TEST_OBJECTS))))

OBJECTS = $(addsuffix .o, $(basename $(OBJECT_LIST)))

# check to see if this is a debug build
ifneq (,$(findstring -DDEBUG,$(CFLAGS)))
	TEST_OBJ_DIR = $(DEBUG_OBJ_DIR)
	DEP_DIR = $(DEBUG_DEP_DIR)
	EXECUTABLE = $(DEBUG_EXECUTABLE)
	OBJECT_FILE_NAMES = $(addprefix $(PROJECT_DIRECTORY)objs/debug/, $(subst  /,+.,$(addprefix $(PROJECT_DIRECTORY),$(OBJECTS))))
	TEST_OBJECTS = $(DEBUG_TEST_OBJECTS)
else
	TEST_OBJ_DIR = $(RELEASE_OBJ_DIR)
	DEP_DIR = $(RELEASE_DEP_DIR)
	EXECUTABLE = $(RELEASE_EXECUTABLE)
	OBJECT_FILE_NAMES = $(addprefix $(PROJECT_DIRECTORY)objs/release/, $(subst  /,+.,$(addprefix $(PROJECT_DIRECTORY),$(OBJECTS))))
	TEST_OBJECTS = $(RELEASE_TEST_OBJECTS)
endif


DEP_TEMP = $(DEP_DIR)temp/
HEAD_PATH = $(PROJECT_DIRECTORY)

TEST_FRAMEWORK_HEADER = $(PROJECT_DIRECTORY)external/google_test/include/
TEST_FRAMEWORK_LIBRARY = $(PROJECT_DIRECTORY)external/google_test/google_test.a

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_TEMP)$*.Td
MOV_DEPS = mv -f $(DEP_TEMP)$*.Td $(DEP_DIR)$*.d
CXX = $(COMP) -I $(HEAD_PATH) -isystem $(TEST_FRAMEWORK_HEADER) $(CFLAGS) -Winvalid-pch $(DEPFLAGS) -std=c++14

DEPFILES := $(patsubst %,$(DEP_DIR)%.d,$(basename $(notdir $(TEST_OBJECTS))))

all: $(EXECUTABLE)

$(EXECUTABLE): $(TEST_OBJECTS) $(OBJECT_FILE_NAMES) $(TEST_FRAMEWORK_LIBRARY) $(TEST_OBJ_DIR)test_lib.a
	$(CXX) -o $@ $^ $(LIBRARIES)
	{ $(EXECUTABLE) 2> /dev/null ; [ $$? -eq 0 ] || touch $(CPP_FILE_NAME); }

.SECONDEXPANSION:
$(TEST_OBJ_DIR)%.o: $$(subst  +.,/, %.cpp) $(DEP_DIR)%.d
	$(CXX) -c -o $@ $<
	$(MOV_DEPS)

.PRECIOUS: $(DEP_DIR)%.d %.h.gch

$(DEP_DIR)%.d: ;

.PHONY: clean
clean:
	rm -f $(RELEASE_TEST_OBJECTS)
	rm -f $(DEBUG_TEST_OBJECTS)

	rm -f $(DEBUG_DEPFILES)
	rm -f $(RELEASE_DEPFILES)

	rm -f $(DEBUG_EXECUTABLE)
	rm -f $(RELEASE_EXECUTABLE)

-include $(DEPFILES)
