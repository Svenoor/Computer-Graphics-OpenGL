# Makefile
CC=g++
CFLAGS=-Iinclude -std=c++11 -g
LIBS=-lglut -lGLEW -lGL -lGLU

# Default target executed when no arguments are given to make.
default_target: project
.PHONY : default_target

OBJS = main.o init.o display.o input.o objects.o globals.o common/InitShader.o

project: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c $<

init.o: init.cpp
	$(CC) $(CFLAGS) -c $<

display.o: display.cpp
	$(CC) $(CFLAGS) -c $<

input.o: input.cpp
	$(CC) $(CFLAGS) -c $<

objects.o: objects.cpp
	$(CC) $(CFLAGS) -c $<

globals.o: globals.cpp
	$(CC) $(CFLAGS) -c $<

common/InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f project *.o *~ *.out

