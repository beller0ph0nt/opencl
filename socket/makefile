CLIENT_OBJ = netclient.o
SERVER_OBJ = netserver.o

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

.PHONY: all

all: client server

client: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(LIBPATH) $(LIB)
server: $(SERVER_OBJ)
	$(CC) -o $@ $^ $(LIBPATH) $(LIB)

netclient.o: netclient.c
	$(CC) -c -o $@ $^
netserver.o: netserver.c
	$(CC) -c -o $@ $^

clear:
	rm -f *.o client server