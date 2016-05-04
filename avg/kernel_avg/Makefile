avg: main.o context.o program.o kernel.o kernel_avg.o err.o free.o file.o
	$(LD) $(LDFLAGS) -o $@ $^ -lOpenCL -lpthread
main.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c
context.o: context.c context.h
	$(CC) -c $(CFLAGS) -o $@ context.c
program.o: program.c program.h
	$(CC) -c $(CFLAGS) -o $@ program.c
kernel.o: kernel.c kernel.h
	$(CC) -c $(CFLAGS) -o $@ kernel.c
kernel_avg.o: kernel_avg.c kernel_avg.h
	$(CC) -c $(CFLAGS) -o $@ kernel_avg.c
err.o: err.c err.h
	$(CC) -c $(CFLAGS) -o $@ err.c
free.o: free.c free.h
	$(CC) -c $(CFLAGS) -o $@ free.c
file.o: file.c file.h
	$(CC) -c $(CFLAGS) -o $@ $<
clean:
	rm -f *.o avg