//  (Windows) g++ -o facedetect{,.cpp} -lopengl32 -lfreeglut (need opencv and dlib)
//  (Linux/Mac) g++ -o facedetect{,.cpp} -I$DLIBDIR `pkg-config opencv --cflags --libs` -LDLIBDIR -ldlib -lglut -lGLU -lGL
//
// This sample shows how to use the dlib's facedetection feature.
//

#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>
#include <chrono>

#include <dlib/opencv.h>
#include "opencv2/highgui/highgui.hpp"
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;

void keyboard(unsigned char key, int x, int y);
void display(void);
void idle(void);
void mouse(int button, int state, int x, int y);
void resize(int w, int h);

GLuint fonts;
GLuint texId;
GLfloat texu, texv, aspect;
cv::VideoCapture cap;
cv::Mat frame;
frontal_face_detector detector;
shape_predictor pose_model;
float winWidth, winHeight, winAspect;

void releaseResources()
{
  cap.release();
}

void myInitFont(void *fontId)
{
  int i;

  fonts = glGenLists(256);
  for (i = 0; i < 256; i++) {
      glNewList(fonts+i, GL_COMPILE);
      glutBitmapCharacter(fontId, i);
      glEndList();
  }
  glListBase(fonts);
}

void myDrawString(char *buf)
{
  int length;
  length = strlen(buf);
  glCallLists(length, GL_UNSIGNED_BYTE, buf);
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
  int i, j;
  char msg[256];
  chrono::system_clock::time_point tp1, tp2;
  chrono::duration<double> elapsed1, elapsed2;

  cap >> frame;
  if (frame.empty()) {
    releaseResources();
    exit(0);
  }

  // This does not copy anything.
  cv_image<bgr_pixel> cimg(frame);

  tp1 = chrono::system_clock::now();
  // Detect faces
  std::vector<rectangle> faces = detector(cimg);
  tp2 = chrono::system_clock::now();
  elapsed1 = chrono::duration_cast<chrono::milliseconds>(tp2 - tp1);

  // Find the pose of each face
  std::vector<full_object_detection> shapes;
  for (i = 0; i < faces.size(); ++i)
    shapes.push_back(pose_model(cimg, faces[i]));
  tp1 = chrono::system_clock::now();
  elapsed2 = chrono::duration_cast<chrono::milliseconds>(tp1 - tp2);

#if 0
  //printf("num face = %d, num (parts = %d\n", faces.size(), shapes[0].num_parts());
  if (faces.size())
    printf("%ld %ld %ld %ld\n",
      faces[0].left(), faces[0].right(),
      faces[0].top(), faces[0].bottom());
#endif

  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glEnable(GL_TEXTURE_2D);
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

  if (faces.size()) {
    for (j = 0; j < faces.size(); j++) {
      GLfloat left, right, top, bottom;
      left = (GLfloat)faces[j].left();
      right = (GLfloat)faces[j].right();
      top = (GLfloat)faces[j].bottom();
      bottom = (GLfloat)faces[j].top();
      glDisable(GL_TEXTURE_2D);
      glColor3f(0.0f, 1.0f, 0.0f);
      glBegin(GL_LINE_STRIP);
        glVertex2f((left / winWidth - 0.5f) * winAspect, (winHeight - top) / winHeight - 0.5f);
        glVertex2f((right / winWidth - 0.5f) * winAspect, (winHeight - top) / winHeight - 0.5f);
        glVertex2f((right / winWidth - 0.5f) * winAspect, (winHeight - bottom) / winHeight - 0.5f);
        glVertex2f((left / winWidth - 0.5f) * winAspect, (winHeight - bottom) / winHeight - 0.5f);
        glVertex2f((left / winWidth - 0.5f) * winAspect, (winHeight - top) / winHeight - 0.5f);
      glEnd();

      glColor3f(1.0f, 1.0f, 1.0f);
      glBegin(GL_POINTS);
        for (i = 0; i < shapes[j].num_parts(); i++) {
          GLfloat px = shapes[j].part(i)(0);
          GLfloat py = shapes[j].part(i)(1);
          glVertex2f((px / winWidth - 0.5f) * winAspect, (winHeight - py) / winHeight - 0.5f);
        }
      glEnd();
    }
    //printf("%ld %ld\n", shapes[0].part(0)(0), shapes[0].part(0)(1));
  }

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.5f, winWidth - 0.5, 0.5f, winHeight - 0.5, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glColor3f(1.0f, 0.0f, 0.0f);
  sprintf(msg, "face detect: %.3f", elapsed1);
  glRasterPos2i(10, 10);
  myDrawString(msg);
  sprintf(msg, "face pose: %.3f", elapsed2);
  glRasterPos2i(10, 20);
  myDrawString(msg);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

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

  myInitFont(GLUT_BITMAP_8_BY_13);
}

void resize(int w, int h)
{
  static bool first = true;
  if (first) {
  initGL(w, h);
    first = false;
  }
  winWidth = (float)w;
  winHeight = (float)h;
  winAspect = winWidth / winHeight;
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#if 1
  gluPerspective(90.0f, (float)w / (float)h, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
#else
  glOrtho(-0.5f * winAspect, 0.5f * winAspect, -0.5f, 0.5f, 1.0f, -1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
#endif
}

int main(int argc, char** argv)
{
  cap.open(0);

  if (!cap.isOpened())
  {
      std::cerr << "***Could not initialize capturing...***" << std::endl;
      return -1;
  }

  int cap_w, cap_h;
  cap_w = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
  cap_h = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  std::cout << "Capture w,h = " << cap_w << ", " << cap_h << "\n";

  detector = get_frontal_face_detector();
  deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

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
