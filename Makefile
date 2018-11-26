# Makefile for the smash program
CC = g++
CFLAGS = -g -Wall -Wno-write-strings
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
signals.o: signals.c signals.h
commands.o: commands.c commands.h
smash.o: smash.cpp commands.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

