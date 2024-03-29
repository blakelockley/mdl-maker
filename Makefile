# Usage:
# make        		# compile sample
# make clean  		# remove output files

CC = gcc
CFLAGS = -Wall -g -Iincludes -Isrc
LFLAGS = libglfw.3.dylib libfreetype.6.dylib cimgui.dylib -framework OpenGL -framework Cocoa -framework IOKit

TARGET = main
SRCS   = ${wildcard src/**/*.c src/*.c}

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LFLAGS) $(SRCS) -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)
