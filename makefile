.PHONY:clean

all:
	gcc -o init init.c
	gcc -o main main.c
clean:
	rm init main