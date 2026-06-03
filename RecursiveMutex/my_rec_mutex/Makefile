CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99 -O2
LDFLAGS = -pthread

# Source files
SOURCES = rec_mutex.c rec_mutex_test.c
HEADERS = rec_mutex.h
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = rec_mutex_test

# Phony targets
.PHONY: all clean run test

# Default target
all: $(EXECUTABLE)

# Build the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✓ Build successful: $(EXECUTABLE)"

# Compile object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

# Run the test suite
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Alias for run
test: run

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	@echo "✓ Clean complete"

# Display usage
help:
	@echo "Available targets:"
	@echo "  make           - Build the test executable"
	@echo "  make run       - Build and run the tests"
	@echo "  make test      - Same as 'make run'"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make help      - Display this help message"
