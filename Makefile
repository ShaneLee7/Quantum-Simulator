CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -D_GNU_SOURCE -D_USE_MATH_DEFINES
SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = quantum_simulator

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lm

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: test
test: $(TARGET)
	./$(TARGET)