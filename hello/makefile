CC = $(CROSS_COMPILE)gcc
CFLAGS = -v
LIBS = -lOpenCL

hello: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
clean:
	rm -f *.o hello