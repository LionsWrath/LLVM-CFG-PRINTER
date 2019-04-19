SOURCE_DIR=$(pwd)

CC=clang
CFLAGS=-I${HOME}/llvm/build/9.0.0/include
LDFLAGS=


all: $(patsubst %.c, %.bc, $(wildcard *.c))

%.bc: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -O0 -emit-llvm -o $@ -c $^

clean:
	rm *.bc
	rm *.dot
	rm *.pdf
