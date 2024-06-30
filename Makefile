# Makefile

# Define directories
SRCDIR := src/
OBJDIR := obj/
INCDIR := include/

# Source files
CPP_SOURCE := $(wildcard $(SRCDIR)*.cpp)
S_SOURCE := $(wildcard $(SRCDIR)*.S)

# Use patsubst to generate object files with prefix
CPP_OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(CPP_SOURCE))
S_OBJECTS := $(patsubst $(SRCDIR)%.S,$(OBJDIR)%.o,$(S_SOURCE))
OBJECTS := $(S_OBJECTS) $(CPP_OBJECTS)

# Compiler and flags
CC = g++
CFLAGS = -I$(INCDIR) -g

# Target
minielf: $(OBJECTS)
	$(CC) -o $@ $^

# Rule for building object files
$(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o : $(SRCDIR)%.S
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) minielf

