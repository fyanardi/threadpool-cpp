# define the C compiler to use
CC = g++

# define any compile-time flags
# -g generates debug information
CFLAGS = -Wall -g -std=c++11

# compiler flags (include and libraries)
INCLUDES = -I..
LFLAGS =
LIBS = -lstdc++ -lpthread

# define the C source files
SRCS = threadpool-main.cpp

# object files to create for the executable
# below we are replacing the suffix .cpp of all words in the macro SRCS
# with the .o suffix
OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = threadpool-main

.PHONY: clean

all:    $(MAIN)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

