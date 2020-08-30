PROGS	= simple capture glfwtest facedetect
ALL:	$(PROGS)

CC = gcc
CXX = g++

ifeq ($(shell uname),Darwin)
CXXFLAGS = `pkg-config opencv4 --cflags`
CXXFLAGS += `pkg-config freeglut --cflags`
endif

ifeq ($(shell uname),MINGW64_NT-10.0-18363)
CXXFLAGS = `pkg-config opencv4 --cflags`
CXXFLAGS += `pkg-config freeglut --cflags`
else
CXXFLAGS = `pkg-config opencv4 --cflags`
endif
CXXFLAGS += -D_THREAD_SAFE
CXXFLAGS +=  -O3 -g
CXXFLAGS +=  -std=c++11

DLIBDIR = /home/axengr/trees/dlib-19.19/

ifeq ($(shell uname),Darwin)
LIBS = `pkg-config opencv4 --libs`
#LIBS += `pkg-config freeglut --libs`
LIBS += -framework GLUT -framework OpenGL -framework Cocoa
LIBS += -L/opt/X11/lib
endif
ifeq ($(shell uname),MINGW64_NT-10.0-18363)
LIBS = `pkg-config opencv4 --libs`
LIBS += `pkg-config freeglut --libs`
#LIBS += -lGLU -lGL
LIBS += -lglu32 -lopengl32
else
LIBS = `pkg-config opencv4 --libs`
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

facedetect.o: facedetect.cpp
	rm -f facedetect.o
	$(CXX) -DUSE_AVX_INSTRUCTIONS=ON -I$(DLIBDIR) $(CXXFLAGS) -c -o $@ facedetect.cpp


#facedetect: facedetect.o
#	rm -f $@
#	$(CXX) -DUSE_AVX_INSTRUCTIONS=ON -I$(DLIBDIR) $(DLIBDIR)/dlib/all/source.cpp -o $@ $@.o $(LDFLAGS) -lpthread -lX11

facedetect: facedetect.o
	rm -f $@
	$(CXX) -I$(DLIBDIR) -o $@ $@.o $(LDFLAGS) $(DLIBDIR)/build/dlib/libdlib.a -lpthread -lX11

glfwtest.o: glfwtest.cpp
	rm -f glfwtest.o
	$(CXX) `pkg-config glfw3 --cflags` $(CXXFLAGS) -c -o $@ glfwtest.cpp

glad.o: glad.cpp
	rm -f glad.o
	$(CXX) $(CXXFLAGS) -c -o $@ glad.cpp
	
glfwtest: glfwtest.o glad.o
	rm -f $@
	$(CXX) -o $@ $@.o glad.o $(LDFLAGS) `pkg-config glfw3 --libs` -lGL -lGLU -ldl

clean:
	rm -f $(PROGS) *.o core
