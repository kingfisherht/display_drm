FLAGS=`pkg-config --cflags --libs libdrm`
FLAGS+=-Wall -O0 -g
FLAGS+=-D_FILE_OFFSET_BITS=64

all:
	gcc -o modeset-single-buffer modeset-single-buffer.c $(FLAGS)
	gcc -o modeset-double-buffer modeset-double-buffer.c $(FLAGS)
	gcc modeset-page-flip.c $(FLAGS)
clean:
	rm a.out