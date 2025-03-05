# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -IQueue  # Debugging enabled

# Directories
SRC_DIRS = Src Queue
OBJ_DIR = build
BIN_DIR = bin

# Find source files
SRCS = $(wildcard Src/*.c Queue/*.c)
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

# Output Binary
TARGET = $(BIN_DIR)/producer_consumer

# Create necessary directories before compiling
$(shell mkdir -p $(OBJ_DIR)/Producer_consumer $(OBJ_DIR)/Queue $(BIN_DIR))

# Default Rule
all: $(TARGET)

# Compile the binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile each .c file into .o
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)  # Ensure the directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the executable
run: all
	./$(TARGET)

# Debug with GDB
debug: all
	gdb --args ./$(TARGET)
