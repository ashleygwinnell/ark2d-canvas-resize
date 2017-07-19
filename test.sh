# compile: 
#	make
# usage: 
# 	ark2d-canvas-resize pixels_x pixels_y align_x align_y relative input_image output_image

./ark2d-canvas-resize 2 2 0 0 true test.png output/test+2x2-centered.png
./ark2d-canvas-resize 6 6 0 0 true test.png output/test+6x6-centered.png

./ark2d-canvas-resize 2 2 -1 -1 true test.png output/test+2x2-tl.png
./ark2d-canvas-resize 6 6 -1 -1 true test.png output/test+6x6-tl.png

./ark2d-canvas-resize 2 2 1 1 true test.png output/test+2x2-br.png
./ark2d-canvas-resize 6 6 1 1 true test.png output/test+6x6-br.png

./ark2d-canvas-resize -2 -2 0 0 true test.png output/test-2x2-centered.png
./ark2d-canvas-resize -2 -2 -1 -1 true test.png output/test-2x2-tl.png
./ark2d-canvas-resize -2 -2 1 1 true test.png output/test-2x2-br.png
./ark2d-canvas-resize -9 -9 0 0 true test.png output/test-9x9-centered.png

./ark2d-canvas-resize 20 20 -1 -1 false test.png output/test-absolute-20x20-tl.png
./ark2d-canvas-resize 20 20 0 0 false test.png output/test-absolute-20x20-centered.png
./ark2d-canvas-resize 20 20 1 1 false test.png output/test-absolute-20x20-br.png

./ark2d-canvas-resize 5 5 -1 -1 false test.png output/test-absolute-5x5-tl.png
./ark2d-canvas-resize 5 5 0 0 false test.png output/test-absolute-5x5-centered.png
./ark2d-canvas-resize 5 5 1 1 false test.png output/test-absolute-5x5-br.png

./ark2d-canvas-resize -11 -11 0 0 true test.png output/test-11x11-centered.png