PROGS	= simple capture 
ALL:	$(PROGS)

CC = gcc
CXX = g++

#SRCS	= capture.cpp
#OBJS	= $(SRCS:.cpp=.o)

CXXFLAGS = `pkg-config opencv --cflags`
CXXFLAGS += -D_THREAD_SAFE
CXXFLAGS +=  -O0 -g
CXXFLAGS +=  -std=c++11
#CXXFLAGS =  -Ofast
#CXXFLAGS +=  -g # -DDEBUG
LIBS = `pkg-config opencv --libs`
LIBS += -lglut -lGLU -lGL
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
