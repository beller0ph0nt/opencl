numthreshold: numthreshold.o kernel.o
	gcc -O3 -o $@ numthreshold.o kernel.o -lOpenCL
numthreshold.o: numthreshold.c
	gcc -c -O3 -o $@ numthreshold.c
kernel.o: kernel.c kernel.h
	gcc -c -O3 -o $@ kernel.c
clean:
	rm -f *.o numthreshold