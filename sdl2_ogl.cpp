#include <iostream>
#include <chrono>
#include <math.h>

using namespace std;

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#define WIN_WIDTH     500
#define WIN_HEIGHT    400

SDL_Window *window = nullptr;
SDL_GLContext context;
TTF_Font *font = nullptr;
GLuint texId;

void init(void)
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

  glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
  glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  glEnable(GL_DEPTH_TEST);
}


void render(void)
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
}

int main(int argc, char * argv[])
{
	int status;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  window = SDL_CreateWindow("SDL2 OpenGL sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL);
  if (!window) {
    std::cerr << "Error creating window or renderer: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  context = SDL_GL_CreateContext(window);
  if (!context) {
    std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  init();

  TTF_Init();
  font = TTF_OpenFont("arial.ttf", 12);

  std::cout << "Hot keys:" << std::endl;
  std::cout << "\tESC - quit the program" << std::endl;
  std::cout << "\tp - pause video" << std::endl;

  bool paused = false;
  bool quit = false;

  SDL_Event e;

  chrono::system_clock::time_point start, end;
  chrono::duration<double> elapsed;

  while (!quit)
  {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          quit = true;
          break;

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              quit = true;
              break;

            case SDLK_p:
              paused = !paused;
              break;
          }
          break;
        }
      }

      if (!paused) {
        start = chrono::system_clock::now();
        render();
        end = chrono::system_clock::now();
        elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
        //cout << "filltexture = " << elapsed.count() * 1000 << " ms\n";
      }
      SDL_GL_SwapWindow(window);

      SDL_Delay(10);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
