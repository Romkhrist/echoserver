#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define BUFF_SIZE    1024
#define DEFAULT_PORT 50000

int main(int argc, char* argv[])
{
    int                listener;             //listening socket
    struct sockaddr_in addr; 
    int                port;
	int                connections      = 3; //max connections
    char               buff[BUFF_SIZE];      //buffer
    fd_set             readset;
	int                nfds;
    struct timeval     timeout;

	memset(buff, '\0', sizeof(buff));

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1)
    {
            perror("Server socket failed");
            exit(1);
    }
    fcntl(listener, F_SETFL, O_NONBLOCK);

    addr.sin_family = AF_INET;

	switch(argc)
	{
		case 1:
			port = DEFAULT_PORT;
			break;
		case 2:
			if (atoi(argv[1]) < 50000 || atoi(argv[1]) > 55000)
			{
				printf("Invalid port number. Port number range 50000 - 55000\n");
				close(listener);
				exit(1);
			} 
			else 
            { 
                port = atoi(argv[1]); 
            }
			break;			
		default:
			printf("Error: too many parameters\n");
            close(listener);
			exit(1);
	}
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) !=0)
    {
        printf("Port \"%d\" already in use\n", port);
        for (port = 50000; port <= 55000; port++)
        {
            addr.sin_port = htons(port);
            if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) != 0)
            {
                continue;
            }
            else 
            {
                printf("Server was switched on port '%d'\n", port);
                break;
            }
        }             
    }

    if (listen(listener, connections) != 0)
	{
		perror("Listen failed");
		close(listener);
		exit(1);
	}
	printf("Server is waiting for connections on port: %d...\n", port);
	
    FD_ZERO(&readset);
	FD_SET(listener, &readset);
	nfds = listener + 1;

    while(true)
    {
        timeout.tv_sec  = 60;
        timeout.tv_usec = 0;

		int count = select(nfds, &readset, NULL, NULL, &timeout);
		if (count == -1)
		{
			perror("Select failed");
			close(listener);
			exit(1);
		}
		else if (count == 0)
		{ 
			printf("Timeout expired. No requests for connection\n");
			close(listener);
			exit(1);
		}

		for (int fd = 0; fd < nfds; fd++)
		{
			if (!FD_ISSET(fd, &readset))
            {
				continue;
            }
            
			if (fd == listener)
			{				
				fd = accept(listener, NULL, NULL);
				printf("Accepted client fd: %d\n", fd);
				if (fd >= FD_SETSIZE)
				{
					perror("Too large descriptor number\n");
					close(fd);
					continue;
				}
				fcntl(fd, F_SETFL, O_NONBLOCK);
				FD_SET(fd, &readset);
                
				if (fd >= nfds)
                {
					nfds = fd + 1;
                }
			}
			else 
            {
                int bytes_read = recv(fd, buff, sizeof(buff), 0);
                if (bytes_read == -1)
                {
                    perror("Receive failed");
                    close(fd);
                    close(listener);
                    exit(1);
                } 
                else if (bytes_read == 0)
                {
                    FD_CLR(fd, &readset);
                    close(fd);
                    printf("Disconnected client fd: %d\n", fd);
                    continue;
                }
                
                printf("Received: %d bytes\n", bytes_read);

                int bytes_sent = send(fd, buff, bytes_read, 0);
                if (bytes_sent == -1)
                {
                    perror("Sending failed");
                    close(fd);
                    close(listener);
                    exit(1);
                }
                
                printf("Sent: %d bytes\n", bytes_sent);

                memset(buff, '\0', sizeof(buff));		
			}
		}
	}
	close(listener);
    return 0;
}
