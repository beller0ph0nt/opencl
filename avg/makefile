avg: main.o kernel.o
	gcc -Wall -O3 -o $@ main.o kernel.o -lOpenCL
main.o: main.c
	gcc -c -Wall -O3 -o $@ main.c
kernel.o: kernel.c kernel.h
	gcc -c -Wall -O3 -o $@ kernel.c
clean:
	rm -f *.o average