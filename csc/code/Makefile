.PHONY: all clean

all: fibo.o main.o

clean:
	rm -rf *.o

%.o: %.c
	gcc $< -I ../include -c -o $@
