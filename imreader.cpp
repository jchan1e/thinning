#include "imreader.h"

using namespace std;

int32_t* readfile(char* filename, int* n, int* m)
{
   ifstream infile;
   infile.open(filename, ios::in | ios::binary);

   infile.read((char*)m, sizeof(int32_t));
   infile.read((char*)n, sizeof(int32_t));

   int32_t* arr = (int32_t*)malloc((*n) * (*m) * sizeof(int32_t));
   infile.read((char*)arr, (*n) * (*m) * sizeof(int32_t));
   infile.close();

   return arr;
}

void writefile(char* filename, float** arr, int N, int M)
{
   ofstream outfile;
   outfile.open(filename, ios::out | ios::binary);

   int32_t* texture = (int32_t*)malloc(N*M * sizeof(int32_t));
   for (int i=0; i < N; ++i)
   {
      for (int j=0; j < M; ++j)
      {
         texture[i*M + j] = (int32_t)(arr[i][j] * INT32_MAX);
      }
   }
   outfile.write((char*)texture, N*M * sizeof(int32_t));

   free(texture);
   outfile.close();
}

void writefile(char* filename, int32_t* texture, int N, int M)
{
   ofstream outfile;
   outfile.open(filename, ios::out | ios::binary);

   outfile.write((char*)texture, N*M * sizeof(int32_t));

   outfile.close();
}
