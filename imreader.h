//#ifndef IMREADER_H
//#define IMREADER_H

#include <fstream>
#include <cstdint>

int32_t* readfile(char* filename, int* n, int* m);

void writefile(char* filename, float** arr, int N, int M);

void writefile(char* filename, int32_t* texture, int N, int M);

//#endif
