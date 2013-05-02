#
# Unix/Linux makefile for StarTiger
#

# 
# List of source files
#
GAME_SRCS=main.cpp
GAME_OBJS=$(GAME_SRCS:.cpp=.o)

# Compile and link options
#

CXX=g++
#CXXFLAGS=-Wall -I. -g -DUSE_JPEG
CXXFLAGS=-Wall -I. -g -DUSE_JPEG -D NDEBUG


#
# OpenGL libraries
#
UNAME := $(shell uname)
ifneq (,$(findstring Darwin,$(UNAME)))
	GLLIBS = -framework GLUT -framework OpenGL
else
  ifneq (,$(findstring CYGWIN,$(UNAME)))
	GLLIBS = -lopengl32 -lglu32 -lglut32
  else
	GLLIBS = -lglut -lGLU -lGL
  endif
endif



#
# Compile command
#

#%.o: %.cpp R3Mesh.h
#	    $(CC) $(CPPFLAGS) -c $< -o $@

#
# GNU Make: targets that don't build files

.PHONY: all clean distclean

# Rules encoding targets and dependencies.  By default, the first of
# these is built, but you can also build any individual target by
# passing it to make - e.g., "make imgpro" or "make clean"
#
# Notice that many of the dependencies are implicit (e.g. a .o depends
# on its corresponding .cpp), as are many of the compilation rules.
#

all: startiger

startiger: $(LIBS) $(GAME_OBJS)
	rm -f $@
	$(CXX) $(CXXFLAGS) $^ -lm -o $@ $(LIBS)

clean: 
	rm -f *.o startiger
