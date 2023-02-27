.PHONY: all code clean

all: code fibo

code:
	make --directory=code

fibo:
	gcc code/*.o -I include -o $@

clean:
	make clean --directory=code
