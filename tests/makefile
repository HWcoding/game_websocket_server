#absolute paths are necessary for dependency files to work properly
#our test executable will have the same name as the cpp file containing the tests
EXECUTABLE_NAME = $(basename $(notdir $(FILENAME)))
LIBRARIES = -lpthread -lstdc++ -lm

PROJECT_DIRECTORY = $(abspath ../)/

COMP = g++
CFLAGS = -DDEBUG -Og -g3 -fno-omit-frame-pointer -fno-inline

TEST_OBJ_DIR := $(PROJECT_DIRECTORY)tests/objs/
DEP_DIR := $(PROJECT_DIRECTORY)tests/deps/
DEP_TEMP := $(DEP_DIR)temp/
EXECUTABLE := $(PROJECT_DIRECTORY)tests/bin/$(EXECUTABLE_NAME).bin
HEAD_PATH := $(PROJECT_DIRECTORY)

TEST_FRAMEWORK_HEADER := $(abspath $(HOME)/googletest-master/googletest/include)/
TEST_FRAMEWORK_LIBRARY := gtest_main.a
MOCK_FRAMEWORK_HEADER := $(abspath $(HOME)/googletest-master/googlemock/include)/
MOCK_FRAMEWORK_LIBRARY := gtest_main.a

CPP_FILE_NAME = $(abspath $(FILENAME))
TEST_OBJECTS_NAME = $(subst  /,-!-, $(basename $(CPP_FILE_NAME)))
TEST_OBJECTS = $(addsuffix .o, $(addprefix $(TEST_OBJ_DIR), $(TEST_OBJECTS_NAME)))

OBJECTS = $(addsuffix .o, $(basename $(OBJECT_LIST)))

OBJECT_FILE_NAMES = $(addprefix $(PROJECT_DIRECTORY)objs/, $(subst  /,-!-,$(addprefix $(PROJECT_DIRECTORY),$(OBJECTS))))
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_TEMP)$*.Td
MOV_DEPS = mv -f $(DEP_TEMP)$*.Td $(DEP_DIR)$*.d
CXX = $(COMP) -I $(HEAD_PATH) -I $(TEST_FRAMEWORK_HEADER) -I $(MOCK_FRAMEWORK_HEADER) $(CFLAGS) -Winvalid-pch $(DEPFLAGS) -std=c++14

DEPFILES := $(patsubst %,$(DEP_DIR)%.d,$(basename $(notdir $(TEST_OBJECTS))))

all: $(EXECUTABLE)

$(EXECUTABLE): $(TEST_OBJECTS) $(OBJECT_FILE_NAMES) $(TEST_FRAMEWORK_LIBRARY) $(MOCK_FRAMEWORK_LIBRARY) $(TEST_OBJ_DIR)test_lib.a
	$(CXX) -o $@ $^ $(LIBRARIES)
	{ $(EXECUTABLE); [ $$? -eq 0 ] || touch $(CPP_FILE_NAME); }

.SECONDEXPANSION:
$(TEST_OBJ_DIR)%.o: $$(subst  -!-,/, %.cpp) $(DEP_DIR)%.d
	$(CXX) -c -o $@ $<
	$(MOV_DEPS)

.PRECIOUS: $(DEP_DIR)%.d %.h.gch

$(DEP_DIR)%.d: ;

.PHONY: clean
clean:
	rm -f $(TEST_OBJECTS)
	rm -f $(DEPFILES)
	rm -f $(EXECUTABLE)

-include $(DEPFILES)
