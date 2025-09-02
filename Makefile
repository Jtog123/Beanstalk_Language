# C compiler
CC = clang
CFLAGS = -std=c99 -Wall -Wextra -g

# source files
SOURCES = main.c chunk.c memory.c debug.c value.c
OBJECTS = $(SOURCES:.c=.o)
EXEC = program

all: $(EXEC)

# Link object files into executable
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c file into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
