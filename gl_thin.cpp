// just this one set of important functions

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
//#include <unistd.h>

#include "imreader.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <SDL.h>
#include <SDL_OpenGL.h>

using namespace std;

SDL_Window* window = NULL;
SDL_GLContext context;


struct ivec2
{
public:
   int x;
   int y;
   ivec2(int X, int Y)
   {
      x = X;
      y = Y;
   }
   friend ivec2 operator+ (ivec2 left, const ivec2& right)
   {
      left.x += right.x;
      left.y += right.y;
      return left;
   }
};

struct vec2
{
public:
   float x;
   float y;
   vec2(float X, float Y)
   {
      x = X;
      y = Y;
   }
   friend vec2 operator+ (vec2 left, const vec2& right)
   {
      left.x += right.x;
      left.y += right.y;
      return left;
   }
   friend vec2 operator* (vec2 left, const vec2& right)
   {
      left.x *= right.x;
      left.y *= right.y;
      return left;
   }
};

float distance(vec2 v1, vec2 v2)
{
   float dx = v1.x - v2.x;
   float dy = v1.y - v2.y;
   return (dx*dx + dy*dy);
}
float distance(ivec2 v1, vec2 v2)
{
   return distance(vec2(v1.x,v1.y), v2);
}
float distance(vec2 v1, ivec2 v2)
{
   return distance(v1, vec2(v2.x,v2.y));
}
float distance(ivec2 v1, ivec2 v2)
{
   return distance(vec2(v1.x,v1.y), vec2(v2.x,v2.y));
}

float findBorders(float** arr, int N, int M, int x, int y)
{
   if (arr[x][y] == 1.0)
   {
      for (int i=-1; i <= 1; ++i)
      {
         for (int j=-1; j <= 1; ++j)
         {
            int a = x+i;
            int b = y+j;
            if (a >= 0 && a < N && b >= 0 && b < M && arr[a][b] == 0.0)
               return 0.5;
         }
      }
   }
   return arr[x][y];
}

std::vector<ivec2> draw_line(const ivec2 A, const ivec2 B)
{
   std::vector<ivec2> thing;

   int x1 = A.x;
   int y1 = A.y;
   int x2 = B.x;
   int y2 = B.y;
   int dx = x2 - x1;
   int dy = y2 - y1;

   if (abs(dx) >= abs(dy))
   {
      for (int x = x1; x2 - x != 0; x += dx/abs(dx))
      {
         int y = y1 + dy*(x-x1)/dx;
         thing.push_back(ivec2(x,y));
      }
   }
   else //dx < dy
   {
      for (int y = y1; y2 - y != 0; y += dy/abs(dy))
      {
         int x = x1 + dx*(y-y1)/dy;
         thing.push_back(ivec2(x,y));
      }
   }

   return thing;
}

float forceThin(float** arr, const int N, const int M, const int x, const int y, const std::vector<ivec2> borders)
{
   vec2 a(x-0.5,y+0.5);
   vec2 b(x+0.5,y+0.5);
   vec2 c(x+0.5,y-0.5);
   vec2 d(x-0.5,y-0.5);

   vec2 fA(0.0,0.0);
   vec2 fB(0.0,0.0);
   vec2 fC(0.0,0.0);
   vec2 fD(0.0,0.0);

   for (unsigned int i=0; i < borders.size(); ++i)
   {
      if (distance(ivec2(x,y), borders[i]) <= 16)
      {
         std::vector<ivec2> line;
         line = draw_line(ivec2(x,y), borders[i]);

         bool outofsight = false;
         //printf("(%d,%d)\n(%d,%d) %zd\n", x, y, borders[i].x, borders[i].y, line.size());
         for (unsigned int j=0; j < line.size(); ++j)
         {
            //printf("\t%d\n", j);
            outofsight |= arr[line[j].x][line[j].y] == 0.5;
         }

         if (!outofsight)
         {
            float dist = distance(a, borders[i]);
            float dx = a.x - borders[i].x;
            float dy = a.y - borders[i].y;
            fA = fA + vec2(dx/(abs(dx)+abs(dy)) / dist, dy/(abs(dx)+abs(dy)) / dist);

            dist = distance(b, borders[i]);
            dx = b.x - borders[i].x;
            dy = b.y - borders[i].y;
            fB = fB + vec2(dx/(abs(dx)+abs(dy)) / dist, dy/(abs(dx)+abs(dy)) / dist);

            dist = distance(c, borders[i]);
            dx = c.x - borders[i].x;
            dy = c.y - borders[i].y;
            fC = fC + vec2(dx/(abs(dx)+abs(dy)) / dist, dy/(abs(dx)+abs(dy)) / dist);

            dist = distance(d, borders[i]);
            dx = d.x - borders[i].x;
            dy = d.y - borders[i].y;
            fD = fD + vec2(dx/(abs(dx)+abs(dy)) / dist, dy/(abs(dx)+abs(dy)) / dist);
         }
      }
   }

   //S |= fA.x*fB.x <= 0.0 && abs(fA.y)+abs(fB.y) > abs(fA.x)+abs(fB.x);
   //S |= fC.x*fD.x <= 0.0 && abs(fC.y)+abs(fD.y) > abs(fC.x)+abs(fD.x);
   //S |= fA.y*fD.y <= 0.0 && abs(fA.y)+abs(fD.y) > abs(fA.x)+abs(fD.x);
   //S |= fB.y*fC.y <= 0.0 && abs(fB.y)+abs(fC.y) > abs(fB.x)+abs(fC.x);
   //S |= fA.x*fC.x <= 0.0 && fA.y * fC.y <= 0.0;
   //S |= fB.x*fD.x <= 0.0 && fB.y * fD.y <= 0.0;

   //S |= fA.x*fB.x <= 0.0 && fC.x*fD.x <= 0.0 && (abs(fA.x)+abs(fB.x) > abs(fA.y)+abs(fB.y) || abs(fC.x)+abs(fD.x) > abs(fC.y)+abs(fD.y));
   //S |= fA.y*fD.y <= 0.0 && fB.y*fC.y <= 0.0 && (abs(fA.y)+abs(fD.y) > abs(fA.x)+abs(fD.x) || abs(fB.y)+abs(fC.y) > abs(fB.x)+abs(fC.x));
   //S |= fA.x*fC.x <= 0.0 && fA.y*fC.y <= 0.0;
   //S |= fB.x*fD.x <= 0.0 && fB.y*fD.y <= 0.0;

   //dot products to find change in direction
   float dot;
   float min_dot = INFINITY;

   dot = fA.x*fB.x + fA.y*fB.y;
   if (dot < min_dot)
      min_dot = dot;
   dot = fB.x*fC.x + fB.y*fC.y;
   if (dot < min_dot)
      min_dot = dot;
   dot = fA.x*fD.x + fA.y*fD.y;
   if (dot < min_dot)
      min_dot = dot;
   dot = fC.x*fD.x + fC.y*fD.y;
   if (dot < min_dot)
      min_dot = dot;
   dot = fA.x*fC.x + fA.y*fC.y;
   if (dot < min_dot)
      min_dot = dot;
   dot = fB.x*fD.x + fB.y*fD.y;
   if (dot < min_dot)
      min_dot = dot;

   return min_dot;
}

bool init(int w, int h)
{
   bool success = true;

   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
   {
      cerr << "SDL failed to initialize: " << SDL_GetError() << endl;
      success = false;
   }

   window = SDL_CreateWindow("OpenGL", 0,0 , w,h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
   if (window == NULL)
   {
      cerr << "SDL failed to create a window: " << SDL_GetError() << endl;
      success = false;
   }

   context = SDL_GL_CreateContext(window);
   if (context == NULL)
   {
      cerr << "SDL failed to create OpenGL context: " << SDL_GetError() << endl;
      success = false;
   }
   
   //Vsync
   if (SDL_GL_SetSwapInterval(0) < 0)
   {
      cerr << "SDL could not set Vsync: " << SDL_GetError() << endl;
      success = false;
   }

   return success;
}

void extend(float** arr, int N, int M, ivec2 point, std::vector<ivec2> borders, std::queue<ivec2>* skel)
{
   int I = point.x;
   int J = point.y;

   std::vector<ivec2> adj;
   adj.push_back(ivec2(I-1, J+1));
   adj.push_back(ivec2(I  , J+1));
   adj.push_back(ivec2(I+1, J+1));
   adj.push_back(ivec2(I+1, J  ));
   adj.push_back(ivec2(I+1, J-1));
   adj.push_back(ivec2(I  , J-1));
   adj.push_back(ivec2(I-1, J-1));
   adj.push_back(ivec2(I-1, J  ));

   // note: by padding the outer edges of the image with a layer of white,
   //       we guarantee that none of these will walk off the edge.

   int black_count = 0;
   int connectivity = 0;
   for (int i=0; i < 8; ++i)
   {
      if (arr[adj[i].x][adj[i].y] != 1.0)
         ++black_count;
   }
   if (black_count > 2) //not an endpoint
      return;
   else if (black_count == 0)
   {
      arr[I][J] = 1.0;
      return;
   }

   for (int i=1; i <= 8; ++i)
   {
      if (arr[adj[i%8].x][adj[i%8].y] != 1.0
            && arr[adj[(i-1)%8].x][adj[(i-1)%8].y] != 0.0)
         ++connectivity;
   }
   if (connectivity != 1) //not an endpoint
      return;

   ////////
   std::vector<ivec2> edge;
   edge.push_back(ivec2(I-1, J+2));
   edge.push_back(ivec2(I  , J+2));
   edge.push_back(ivec2(I+1, J+2));
   edge.push_back(ivec2(I+2, J+1));
   edge.push_back(ivec2(I+2, J  ));
   edge.push_back(ivec2(I+2, J-1));
   edge.push_back(ivec2(I+1, J-2));
   edge.push_back(ivec2(I  , J-2));
   edge.push_back(ivec2(I-1, J-2));
   edge.push_back(ivec2(I-2, J-1));
   edge.push_back(ivec2(I-2, J  ));
   edge.push_back(ivec2(I-2, J+1));

   for (unsigned int i=0; i < edge.size(); ++i)
   {
      if (arr[edge[i].x][edge[i].y] == 0.5)
         return;
   }
   ////////

   std::vector<ivec2> test;
   for (int i=0; i < 8; ++i)
   {
      if (arr[adj[i].x][adj[i].y] == 0.0)
      {
         if (black_count == 1)
         {
            test.push_back(adj[(i+3)%8]);
            test.push_back(adj[(i+4)%8]);
            test.push_back(adj[(i+5)%8]);
         }
         else
            test.push_back(adj[(i+4)%8]);
      }
   }

   float min_dot = INFINITY;
   int min_index = 0;
   for (unsigned int i=0; i < test.size(); ++i)
   {
      float dot = forceThin(arr, N, M, test[i].x, test[i].y, borders);
      if (dot < min_dot)
      {
         min_dot = dot;
         min_index = i;
      }
   }
   arr[test[min_index].x][test[min_index].y] = 0.0;
   skel->push(test[min_index]);
}

char* ReadText(char* file)
{
   int n;
   char* buffer;
   FILE* f = fopen(file,"r");
   if (!f) {cerr << "Cannot open text file " << file << endl;}
   fseek(f, 0, SEEK_END);
   n = ftell(f);
   rewind(f);
   buffer = (char*) malloc(n+1);
   if (!buffer) {cerr << "Cannot allocate " << n+1 << " bytes for text file " << file << endl;}
   int h = fread(buffer, n, 1, f);
   if (h != 1) {cerr << h << " Cannot read " << n << " bytes for text file " << file << endl;}
   buffer[n] = 0;
   fclose(f);
   return buffer;
}

// this function stolen from class examples
int CreateShader(GLenum type, char* file)
{
   // Create the shader
   int shader = glCreateShader(type);
   // Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader, 1, (const char**) &source, NULL);
   free(source);
   // Compile the shader
   fprintf(stderr, "Compile %s\n", file);
   glCompileShader(shader);
   // Return name (int)
   return shader;
}

// this function stolen (mostly) from class examples
int CreateShaderProg(char* VertFile, char* FragFile)
{
   // Create program
   int prog = glCreateProgram();
   // Create and compile vertex and fragment shaders
   int vert, frag;
   if (VertFile) vert = CreateShader(GL_VERTEX_SHADER,  VertFile);
   if (FragFile) frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   // Attach vertex and fragment shaders
   if (VertFile) glAttachShader(prog,vert);
   if (FragFile) glAttachShader(prog,frag);
   // Link Program
   glLinkProgram(prog);
   // Return name (int)
   return prog;
}

int main(int argc, char** argv)
{
   ////////// read in 2D array of data
   int N = 100;
   int M = 150;

   int32_t* input = NULL;
   float** arr = NULL;

   if (argc > 1)
   {
      int32_t* temp = readfile(argv[1], &N, &M);

      //wrap 24 times
      input = (int32_t*)malloc(M*N * sizeof(int32_t));
      for (int i=0; i < 24; ++i)
      {
         //start at beginning of section
         //jump at N/24
         //jump N places
         for (int m=0; m < M; ++m)
         {
            for (int n=0; n < N/24; ++n)
            {
               int id1 = i*M*N/24 + m*N/24 + n;
               int id2 = i*N/24 + m*N + n;
               input[id1] = temp[id2];
            }
         }
      }

      free(temp);

      N /= 24;
      M *= 24;

      for (int i=0; i < N*M; ++i)
      {
         if (input[i] != 0)
            input[i] = UINT32_MAX;
      }

      arr = (float**)malloc(N*sizeof(float*));
      for (int i=0; i < N; ++i)
      {
         arr[i] = (float*)malloc(M*sizeof(float));
         for (int j=0; j < M; ++j)
         {
            arr[i][j] = (float)(input[i*M + j] != 0.0);
         }
      }
      //free(input);
   }

   else
   {
      for (int i=0; i < N; ++i)
      {
         for (int j=0; j < M; ++j)
         {
            if (((45 < (i+j/8)%80 && (i+j/8)%80 < 55) || (i%66 >= 20 && (j+i/3)%28 >= 20)) && i != N-1 && i != 0 && j != M-1 && j != 0)
               arr[i][j] = 0.0;
            else
               arr[i][j] = 1.0;
            printf("%d", arr[i][j] == 1.0);
         }
         printf("\n");
      }
   }
   ////////// normalize array to floats 0.0 and 1.0

   ////////// Initialize OpenGL/SDL and data texture
   int nn = N;
   int mm = M;
   while (mm > 1800 || nn > 2880)
   {
      cout << nn << " " << mm << endl;
      nn /= 2;
      mm /= 2;
   }
   init(nn, mm);
   cout << nn << " " << mm << endl;

   unsigned int shader = CreateShaderProg(NULL, (char*)"shader.frag");
   unsigned int brader = CreateShaderProg(NULL, (char*)"border.frag");

   unsigned int data;
   glGenTextures(1,&data);
   glBindTexture(GL_TEXTURE_2D, data);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, M, 0, GL_RED, GL_UNSIGNED_INT, input);
   unsigned int image;
   glGenTextures(1,&image);
   glBindTexture(GL_TEXTURE_2D, image);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, M, 0, GL_RGB, GL_UNSIGNED_INT, 0);
   unsigned int borders;
   glGenTextures(1,&borders);
   glBindTexture(GL_TEXTURE_2D, borders);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, M, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

   unsigned int framebuf = 0;
   unsigned int renderbuf = 0;
   glGenFramebuffers(1,&framebuf);
   glGenRenderbuffers(1,&renderbuf);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,image,0);
   
   //////////
   
   // draw the original image to the screen
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, data);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
   glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);
   glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
   glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glFlush();
   SDL_GL_SwapWindow(window);

   //usleep(2 * 1000000);

   //std::vector<ivec2> borders;
   //std::queue<ivec2> skeleton;

//////////////////////////////////////////
//////////////////////////////////////////
//#pragma omp parallel
//  {
//#pragma omp for collapse(2)
//   for (int i=0; i < N; ++i)
//   {
//      for (int j=0; j < M; ++j)
//      {
//         arr[i][j] = findBorders(arr, N, M, i, j);
//         if (arr[i][j] == 0.5)
//         {
//#pragma omp critical
//            borders.push_back(ivec2(i,j));
//            //printf("%zd (%d,%d)\n", borders.size(), i, j);
//         }
//      }
//      //printf("\n");
//   }

   bool quit = false;
   //if (sizeof(ivec2) == sizeof(float)*2)
   //   quit = true;
   SDL_Event event;

   while (!quit)
   {
      while(SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_KEYDOWN:
               switch(event.key.keysym.scancode)
               {
                  case SDL_SCANCODE_Q:
                     quit = true;
                  default:
                     break;
               }
            default:
               break;
         }
      }
      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
      glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);
      glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
      glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      glFlush();
      SDL_GL_SwapWindow(window);

      SDL_Delay(100);
   }
   quit = false;
   //printf("now this\n");

   glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
   //glActiveTexture(GL_TEXTURE1);
   //glBindTexture(GL_TEXTURE_2D, borders);
   //glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, data);
   glUseProgram(brader);
      int id;
      id = glGetUniformLocation(brader, "DX");
      if (id >= 0) glUniform1f(id, 1.0/nn);
      id = glGetUniformLocation(brader, "DY");
      if (id >= 0) glUniform1f(id, 1.0/mm);
      id = glGetUniformLocation(brader, "img");
      if (id >= 0) glUniform1i(id, 0);

      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
      glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);
      glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
      glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
      glEnd();
      glBindTexture(GL_TEXTURE_2D, borders);
      glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,nn,mm,0);
      glDisable(GL_TEXTURE_2D);
      glFlush();
      //SDL_GL_SwapWindow(window);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, borders);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, data);
   glUseProgram(shader);
      id = glGetUniformLocation(shader, "DX");
      if (id >= 0) glUniform1f(id, 1.0/nn);
      id = glGetUniformLocation(shader, "DY");
      if (id >= 0) glUniform1f(id, 1.0/mm);
      id = glGetUniformLocation(shader, "img");
      if (id >= 0) glUniform1i(id, 0);
      id = glGetUniformLocation(shader, "borders");
      if (id >= 0) glUniform1i(id, 1);

      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
      glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);
      glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
      glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
      glEnd();
      glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,nn,mm,0);
      glDisable(GL_TEXTURE_2D);
      glFlush();
      //SDL_GL_SwapWindow(window);

   cout << gluErrorString(glGetError()) << endl;

   SDL_Delay(1000);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, data);
   glUseProgram(0);
   while (!quit)
   {
      while(SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_KEYDOWN:
               switch(event.key.keysym.scancode)
               {
                  case SDL_SCANCODE_Q:
                     quit = true;
                  default:
                     break;
               }
            default:
               break;
         }
      }
      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
      glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);
      glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
      glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      glFlush();
      SDL_GL_SwapWindow(window);
      
      SDL_Delay(100);
   }

//#pragma omp for collapse(2)
//   for (int i=0; i < N; ++i)
//   {
//      for (int j=0; j < M; ++j)
//      {
//         if (arr[i][j] == 0.0 && 0.000 >= forceThin(arr, N, M, i, j, borders))
//         {
//            arr[i][j] = 0.0;
//#pragma omp critical
//            skeleton.push(ivec2(i,j));
//         }
//         else if (arr[i][j] != 0.5)
//            arr[i][j] = 1.0;
//         //printf("%d", (arr[i][j] != 0.0) + (arr[i][j] == 0.5));
//      }
//      //printf("\n");
//   }
  //}
//   while (skeleton.size() > 1)
//   {
//      ivec2 point = skeleton.front();
//      skeleton.pop();
//      extend(arr, M, N, point, borders, &skeleton);
//   }
//////////////////////////////////////////
//////////////////////////////////////////

   if (argc == 1)
   {
      for (int i=0; i < N; ++i)
      {
         for (int j=0; j < M; ++j)
         {
            printf("%d", (arr[i][j] != 0.0) + (arr[i][j] == 0.5));
         }
         printf("\n");
      }
   }
   else
   {
      if (argc == 2)
         writefile(argv[1], input, N, M);
      else //argc >= 3
         writefile(argv[2], input, N, M);
   }

   for (int i=0; i < N; ++i)
      free(arr[i]);
   free(arr);
   free(input);

   SDL_Quit();

   return 0;
}
