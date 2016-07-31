#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define BUF_SIZE 256

int
main(int argc, char** argv)
{
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2)
     {
         fprintf(stderr,"usage: %s <port_number>\n", argv[0]);
         return EXIT_FAILURE;
     }

     int sock = socket(AF_INET, SOCK_STREAM, 0);
     if (socket < 0)
     {
         printf("socket() failed: %s\n", strerror(errno));
         return EXIT_FAILURE;
     }

     memset((char*) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     int port = atoi(argv[1]);
     serv_addr.sin_port = htons(port);

     if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     {
         printf("bind() failed: %s\n", strerror(errno));
         return EXIT_FAILURE;
     }

     printf("listening...\n");
     listen(sock, 1);

     printf("accepting request...\n");
     int clen = sizeof(cli_addr);
     int newsock = accept(sock, (struct sockaddr *) &cli_addr, &clen);
     if (newsock < 0)
     {
        printf("accept() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
     }

     char buf[BUF_SIZE] = { 0 };
     read(newsock, buf, BUF_SIZE-1);
     printf("MSG: %s\n", buf);

     write(newsock, "OK", 2);
     close(newsock);

     close(sock);
}
