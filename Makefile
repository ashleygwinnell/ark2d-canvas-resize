all: ark2d-canvas-resize

ark2d-canvas-resize.o: ark2d-canvas-resize.cpp
	g++ -std=c++11 -c -o ark2d-canvas-resize.o ark2d-canvas-resize.cpp -DNDEBUG `sdl2-config --cflags`

ark2d-canvas-resize: ark2d-canvas-resize.o
	g++ -o ark2d-canvas-resize ark2d-canvas-resize.o -lSDL2_image `sdl2-config --libs`

clean:
	rm -f ark2d-canvas-resize.o ark2d-canvas-resize