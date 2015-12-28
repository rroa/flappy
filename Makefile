CC = clang++
INCLUDES = -I /usr/local/Cellar/glew/1.12.0/include
LIBS = -framework SDL2 -framework OpenGL -lGLEW
CFLAGS = -Wall -c -std=c++11
SOURCES=$(wildcard *.c *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
LDFLAGS = $(LIBS) $(INCLUDES)
EXECUTABLE = main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	rm -f *.o

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)

.PHONY: all clean
