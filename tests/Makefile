SOURCE_DIR=$(pwd)

CC=clang
CFLAGS=-I${HOME}/llvm/build/9.0.0/include
LDFLAGS=


all: $(patsubst %.c, %.bc, $(wildcard *.c))

%.bc: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -emit-llvm -o $@ -c $^

clean:
	rm *.bc
