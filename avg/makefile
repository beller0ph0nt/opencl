avg: main.o kernel.o
	$(CC) $(CFLAGS) -o $@ main.o kernel.o -lOpenCL
main.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c
kernel.o: kernel.c kernel.h
	$(CC) -c $(CFLAGS) -o $@ kernel.c
clean:
	rm -f *.o average