// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 54321
#define MAXLINE 1024

void generateMessage(char *msg, int len)
{
    msg[len - 1] = '\0';
    for (int i = 0; i < len - 1; i++)
        msg[i] = 'A';
}

void runExperiment(int sockfd, struct sockaddr_in servaddr, int msgLen, int numIterations)
{
    char buf_from_server[MAXLINE];
    int n;
    for (int i = 0; i < numIterations; i++)
    {
        char msg[msgLen];
        generateMessage(msg, msgLen);

        sendto(sockfd, msg, msgLen,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, buf_from_server, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &n);

        if (numIterations < 10)
        {
            // Make sure message is being received
            printf("\nMessage echoed back from server:\n\t");
            fputs(buf_from_server, stdout);
        }
        else
        {
            //Print progress
            if (i % 10000 == 0)
            {
                int current = 10 + i / 1000;
                int total = numIterations / 1000;
                printf("%dk/%dk\n", current, total);
            }
        }
    }
}

void runAndTimeExperiments(int sockfd, struct sockaddr_in servaddr)
{
    int numIterations = 100000;
    int msgLen = 10;
    clock_t start = clock();
    runExperiment(sockfd, servaddr, msgLen, numIterations);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTime taken: %fs", time_taken);
    return 0;
}

// Driver code
int main()
{
    int sockfd;
    char buffer[MAXLINE];

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

    runAndTimeExperiments(sockfd, servaddr);
}
