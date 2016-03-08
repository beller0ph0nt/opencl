avg: main.o kernel.o err.o free.o
	$(CC) $(CFLAGS) -o $@ main.o kernel.o err.o free.o -lOpenCL
main.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c
kernel.o: kernel.c kernel.h
	$(CC) -c $(CFLAGS) -o $@ kernel.c
err.o: err.c err.h
	$(CC) -c $(CFLAGS) -o $@ err.c
free.o: free.c free.h
	$(CC) -c $(CFLAGS) -o $@ free.c
clean:
	rm -f *.o avg