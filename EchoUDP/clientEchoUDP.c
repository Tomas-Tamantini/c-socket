// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 54321
#define MAXLINE 1024

void generateMessage(char *msg, int len)
{
    msg[len - 1] = '\0';
    for (int i = 0; i < len - 1; i++)
        msg[i] = 'A';
}

// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char buf_from_server[MAXLINE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    int count = 0;
    int len = 10;
    while (count < 3)
    {
        count++;
        char msg[len];
        generateMessage(msg, len);

        sendto(sockfd, msg, len,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, buf_from_server, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &n);
        printf("\nMessage from server:\n\t");
        buf_from_server[n] = '\0';
        fputs(buf_from_server, stdout);
    }
    return 0;
}
