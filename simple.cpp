//  (Windows) g++ -o simple{,.cpp} -lopengl32 -lfreeglut
//  (Linux/Mac) g++ -o simple{,.cpp} -lglut -lGLU -lGL
#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>

using namespace std;

void keyboard(unsigned char key, int x, int y);
void display(void);
void idle(void);
void mouse(int button, int state, int x, int y);
void resize(int w, int h);

GLuint texId;

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case '\x1B':
      exit(EXIT_SUCCESS);
      break;
  }
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0f, 0.0f, 0.0f);

  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f( 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f( 0.5f,  0.5f);
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

void initGL()
{
  static int width=8, height=8;
  static GLubyte tex1[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 1, 1, 1, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0 };

  GLubyte tex[64][3];
  int i, j;

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      int p = i * width + j;
      if (tex1[(height - i - 1) * width + j]) {
        tex[p][0] = 255; tex[p][1] = 0; tex[p][2] = 0;
      } else {
        tex[p][0] = 255; tex[p][1] = 255; tex[p][2] = 255;
      }
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void resize(int w, int h)
{
  static bool first = true;
  if (first) {
    initGL();
    first = false;
  }
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (float)w / (float)h, 1.0f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(256, 256);
  glutCreateWindow("GLUT Test");
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutIdleFunc(idle);
  glutMainLoop();

  return EXIT_SUCCESS;
}
