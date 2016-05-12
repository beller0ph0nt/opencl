#.PHONY: kernel_avg

OBJ += main.o
OBJ += context.o
OBJ += program.o
OBJ += kernel.o
#OBJ += kernel_avg/kernel.o
OBJ += err.o
OBJ += free.o
OBJ += file.o

OBJ += kernel_avg.o

avg: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ -lOpenCL -lpthread -lm
main.o: main.c
	$(CC) -c $(CFLAGS) -o $@ $<
context.o: context.c context.h
	$(CC) -c $(CFLAGS) -o $@ $<
program.o: program.c program.h
	$(CC) -c $(CFLAGS) -o $@ $<
kernel.o: kernel.c kernel.h
	$(CC) -c $(CFLAGS) -o $@ $<

#kernel_avg/kernel.o:
#	$(MAKE) -C kernel_avg

err.o: err.c err.h
	$(CC) -c $(CFLAGS) -o $@ $<
free.o: free.c free.h
	$(CC) -c $(CFLAGS) -o $@ $<
file.o: file.c file.h
	$(CC) -c $(CFLAGS) -o $@ $<
kernel_avg.o: kernel_avg.c kernel_avg.h kernel_avg_params.h kernel_avg.cl
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
#	$(MAKE) -C kernel_avg clean
	rm -f *.o avg