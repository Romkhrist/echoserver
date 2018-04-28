#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define BUFFSIZE 1024

int main(int argc, char* argv[])
{
     int                    sockfd;
     struct sockaddr_in     addr;
     char                   buff[BUFFSIZE];
     
     memset(buff, '\0', sizeof(buff));
     
     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
     {
         perror("echoclient: socket()");
         exit(1);
     }

     addr.sin_family        = AF_INET;
     addr.sin_port          = htons(atoi(argv[1]));
     addr.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);

     if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
     {
         perror("echoclient: connect()");
         close(sockfd);
         exit(1);
     }
     
     while(true)
     {
        printf("Input a message: \n");
        fgets(buff, sizeof(buff), stdin);
        
        int bytes_sent = send(sockfd, buff, sizeof(buff), 0);
        if(bytes_sent == -1)
        {
        	perror("echoclient: send()");
        	close(sockfd);
        	exit(1);
        }
        
        int bytes_read = recv(sockfd, buff, bytes_sent, 0);
        if(bytes_read == -1)
        {
        	perror("echoclient: recv()");
        	close(sockfd);
        	exit(1);
        }
        else if(bytes_read == 0)
        {
        	printf("echoclient: connection finished\n");
        	break;
        }
        
        printf("Received: %s\n", buff);
        memset(buff, '\0', sizeof(buff));
     }
     
     close(sockfd);
     return 0;
}
