PROGS	= simple capture 
ALL:	$(PROGS)

CC = gcc
CXX = g++

ifeq ($(shell uname),Darwin)
CXXFLAGS = `pkg-config opencv4 --cflags`
CXXFLAGS += `pkg-config freeglut --cflags`
else
CXXFLAGS = `pkg-config opencv --cflags`
endif
CXXFLAGS += -D_THREAD_SAFE
CXXFLAGS +=  -O0 -g
CXXFLAGS +=  -std=c++11

ifeq ($(shell uname),Darwin)
LIBS = `pkg-config opencv4 --libs`
#LIBS += `pkg-config freeglut --libs`
LIBS += -framework GLUT -framework OpenGL -framework Cocoa
else
LIBS = `pkg-config opencv --libs`
LIBS += -lglut -lGLU -lGL
endif
LDFLAGS = $(LIBS)

.cpp.o:
	rm -f $*.o
	$(CXX) -c $(CXXFLAGS) $*.cpp

#$(PROGS): $(OBJS)
#	rm -f $@
#	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

simple: simple.o
	rm -f $@
	$(CXX) -o $@ $@.o $(LDFLAGS)

capture: capture.o
	rm -f $@
	$(CXX) -o $@ $@.o $(LDFLAGS)

clean:
	rm -f $(PROGS) $(OBJS) core
