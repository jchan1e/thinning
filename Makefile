CFLG=-O3 -Wall
SDL_FLG=-O3 -Wall $(shell sdl2-config --cflags) -DGL_GLEXT_PROTOTYPES
LIBS=-lGLU -lGL -lm
SDL_LIB=-lGLU -lGL -lm $(shell sdl2-config --libs)
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework OpenGL
SDL_FLG=-O3 -Wall -Wno-deprecated-declarations $(shell sdl2-config --cflags)
SDL_LIB=-framework OpenGL $(shell sdl2-config --libs)
endif

all: thin gl_thin

imreader.o: imreader.cpp imreader.h
	g++-5 -std=c++11 $(CFLG) -g -c $< -o $@
thin.o: thin.cpp imreader.h
	g++-5 -std=c++11 $(CFLG) -g -c $< -o $@ -fopenmp
gl_thin.o: gl_thin.cpp imreader.h shader.frag border.frag
	g++-5 -std=c++11 $(SDL_FLG) -g -c $< -o $@
thin: thin.o imreader.o
	g++-5 -std=c++11 $(CFLG) -g $^ -o $@ -fopenmp
gl_thin: gl_thin.o imreader.o
	g++-5 -std=c++11 $(SDL_FLG) -g $^ -o $@ $(SDL_LIB)

clean:
	rm -rf thin gl_thin *.o *.a *.dSYM
