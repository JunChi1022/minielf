# Makefile

# Define directories
SRCDIR := src/
OBJDIR := obj/
INCDIR := include/

# Source files
SOURCES := $(wildcard $(SRCDIR)*.cpp)

# Use patsubst to generate object files with prefix
OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SOURCES))

# Compiler and flags
CC = g++
CFLAGS = -I$(INCDIR) -g

# Target
minielf: $(OBJECTS)
	$(CC) -o $@ $^

# Rule for building object files
$(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) minielf
