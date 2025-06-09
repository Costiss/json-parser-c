# Compiler and flags
CC := gcc
CFLAGS := -Iinclude -Wall -MMD

# Project structure
SRCDIR := src
INCDIR := include
OBJDIR := obj
TARGET := my_program

# Find all .c files in the source directory
SOURCES := $(wildcard $(SRCDIR)/*.c)

# Create object file names in the object directory
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Create dependency file names
DEPS := $(OBJECTS:.o=.d)

# Default target
all: $(TARGET)

# Linking the program
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compiling source files and creating object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Include the dependency files
-include $(DEPS)

# Run the program
run: all
	./$(TARGET)

# Watch for file changes and automatically re-compile and run
# Requires 'inotify-tools' to be installed
watch:
	@echo "Watching for changes in $(SRCDIR) and $(INCDIR)..."
	@while true; do \
		inotifywait -q -e modify,create,delete -r $(SRCDIR) $(INCDIR); \
		make run; \
	done

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean run watch
