//  (Windows) g++ -o simple{,.cpp} -lopengl32 -lfreeglut
//  (Linux/Mac) g++ -o simple{,.cpp} -lglut -lGLU -lGL
#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"

using namespace std;

void keyboard(unsigned char key, int x, int y);
void display(void);
void idle(void);
void mouse(int button, int state, int x, int y);
void resize(int w, int h);

GLuint texId;
GLfloat texu, texv, aspect;
cv::VideoCapture cap;
cv::Mat frame;

void releaseResources()
{
  cap.release();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case '\x1B':
      releaseResources();
      exit(EXIT_SUCCESS);
      break;
  }
}

void display()
{
  cap >> frame;
  if (frame.empty()) {
    releaseResources();
    exit(0);
  }

  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glBindTexture(GL_TEXTURE_2D, texId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows,
    GL_BGR, GL_UNSIGNED_BYTE, (void *)frame.ptr(0));

  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f * aspect,  0.5f);
    glTexCoord2f(0.0f, texv); glVertex2f(-0.5f * aspect, -0.5f);
    glTexCoord2f(texu, 0.0f); glVertex2f( 0.5f * aspect,  0.5f);
    glTexCoord2f(texu, texv); glVertex2f( 0.5f * aspect, -0.5f);
  glEnd();

  glFlush();
  glutSwapBuffers();
}

void idle(void)
{
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{

}

void initGL(int w, int h)
{
  int i, j;
  int width, height;

  width = 1;
  while(width < w) { width <<= 1; }
  height = 1;
  while(height < h) { height <<= 1; }
  texu = (float)w / (float)width;
  texv = (float)h / (float)height;
  aspect = (float)w / (float)h;
  cout << "texuv = " << texu << ", " << texv << "\n";

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void resize(int w, int h)
{
  static bool first = true;
  if (first) {
  initGL(w, h);
    first = false;
  }
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (float)w / (float)h, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

int main(int argc, char** argv)
{
  cap.open(-1);

  if (!cap.isOpened())
  {
      std::cerr << "***Could not initialize capturing...***" << std::endl;
      return -1;
  }

  int cap_w, cap_h;
  cap_w = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
  cap_h = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  std::cout << "Capture w,h = " << cap_w << ", " << cap_h << "\n";

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(cap_w, cap_h);
  glutCreateWindow("GLUT Test");
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMainLoop();

  return EXIT_SUCCESS;
}
