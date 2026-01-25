CC = gcc
CFLAGS = -Wall -g -std=c11 -D_CRT_DECLARE_NONSTDC_NAMES -D_isatty=_isatty -D_fileno=_fileno
INCLUDES = -Iinclude

# Explicitly listed source files to ensure correct linking
SOURCES = src/algorithms.c \
          src/cli.c \
		  src/graph.c \
		  src/io.c \
		  src/main.c

# Generate object file names
OBJECTS = $(SOURCES:.c=.o)


# Directories
SRC_DIR = src
BUILD_DIR = build

# Name of the final executable
TARGET = route_planner

# Generate Object File names and redirect them to the BUILD_DIR
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Default target
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link everything into the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET).exe $(OBJECTS)

# Compile .c files to .o files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up
clean:
	-del /Q $(subst /,\,$(OBJECTS)) $(TARGET).exe 2>nul || rm -f $(OBJECTS) $(TARGET).exe

.PHONY: clean

TEST_DIR = tests
TEST_BIN = build/run_tests

CFLAGS = -Wall -g -std=c11

# List of the algorithm object files needed for testing
TEST_DEPENDENCIES = $(BUILD_DIR)/graph.o $(BUILD_DIR)/algorithms.o $(BUILD_DIR)/io.o $(BUILD_DIR)/cli.o

tests: $(BUILD_DIR) $(TEST_BIN)
	./$(TEST_BIN)

# Link the test files with the already compiled objects
$(TEST_BIN): tests/run_all_tests.c tests/test_graph.c tests/test_algorithms.c $(TEST_DEPENDENCIES)
	$(CC) $(CFLAGS) -Iinclude -Itests -o $@ $^

# The Test Target
TEST_OBJECTS = $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS))

.PHONY: tests