.PHONY: all run clean

all: test

test: test.c
	g++ test.c -o test

run: test
	./test

clean:
	rm test
