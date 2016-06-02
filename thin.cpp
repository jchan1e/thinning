// just this one set of important functions

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <queue>
#include <math.h>

#include "imreader.h"

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


int main(int argc, char** argv)
{
   ////////// read in 2D array of data
   int N = 100;
   int M = 150;

   int32_t* input;
   float** arr;

   if (argc > 1)
   {
      input = readfile(argv[1], &N, &M);

      arr = (float**)malloc(N*sizeof(float*));
      for (int i=0; i < N; ++i)
      {
         arr[i] = (float*)malloc(M*sizeof(float));
         for (int j=0; j < M; ++j)
         {
            arr[i][j] = (float)(input[i*M + j] != 0.0);
         }
      }
      free(input);
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


   std::vector<ivec2> borders;
   std::queue<ivec2> skeleton;

#pragma omp parallel
  {
#pragma omp for collapse(2)
   for (int i=0; i < N; ++i)
   {
      for (int j=0; j < M; ++j)
      {
         arr[i][j] = findBorders(arr, N, M, i, j);
         if (arr[i][j] == 0.5)
         {
#pragma omp critical
            borders.push_back(ivec2(i,j));
            //printf("%zd (%d,%d)\n", borders.size(), i, j);
         }
      }
      //printf("\n");
   }



#pragma omp for collapse(2)
   for (int i=0; i < N; ++i)
   {
      for (int j=0; j < M; ++j)
      {
         if (arr[i][j] == 0.0 && 0.000 >= forceThin(arr, N, M, i, j, borders))
         {
            arr[i][j] = 0.0;
#pragma omp critical
            skeleton.push(ivec2(i,j));
         }
         else if (arr[i][j] != 0.5)
            arr[i][j] = 1.0;
         //printf("%d", (arr[i][j] != 0.0) + (arr[i][j] == 0.5));
      }
      //printf("\n");
   }
  }
   while (skeleton.size() > 1)
   {
      ivec2 point = skeleton.front();
      skeleton.pop();
      extend(arr, M, N, point, borders, &skeleton);
   }

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
         writefile(argv[1], arr, N, M);
      else //argc >= 3
         writefile(argv[2], arr, N, M);
   }

   for (int i=0; i < N; ++i)
      free(arr[i]);
   free(arr);

   return 0;
}
