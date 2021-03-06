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
#define MAXLINE 32768

void generateMessage(char *msg, int len)
{
    msg[len - 1] = '\0';
    for (int i = 0; i < len - 1; i++)
        msg[i] = 'A' + (random() % 26);
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
            printf("\nMessage echoed back from server:\n\t");
            fputs(buf_from_server, stdout); // Make sure message is being received
        }
        else if (i % 1 == 0)
        {
            int total = numIterations / 1000;
            printf("%d/%dk\n", i, total); //Print progress
        }
    }
}

void setMessageSizes(int *msgLengths)
{
    for (int i = 0; i < 42; i++)
    {
        if (i == 0)
            msgLengths[i] = 1;
        else if (i < 10)
            msgLengths[i] = 100 * i;
        else
            msgLengths[i] = 1024 * (i - 9);
    }
}

void runAndTimeExperiments(int sockfd, struct sockaddr_in servaddr)
{
    //Setup CSV file and its header
    FILE *resultsCSV = fopen("experiment_results.csv", "w");
    fprintf(resultsCSV, "Message size (B); Num iterations; Total time (s)\n");

    int numIterations = 100000;
    int msgLengths[42];
    setMessageSizes(msgLengths);

    for (int i = 0; i < 42; i++)
    {
        int msgLen = msgLengths[i];
        printf("Experiment %d - Message size %dB\n\n", i + 1, msgLen);
        clock_t start = clock();
        runExperiment(sockfd, servaddr, msgLen, numIterations);
        clock_t end = clock();
        double timeTaken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time taken: %fs", timeTaken);
        printf("\n-----------------------------\n");
        fprintf(resultsCSV, "%d;%d;%f\n", msgLen, numIterations, timeTaken);
    }
    fclose(resultsCSV);
}

// Driver code
int main(int argc, char *argv[])
{
    char *host;
    int sockfd;
    char buffer[MAXLINE];

    struct sockaddr_in servaddr;

    if (argc == 2)
        host = argv[1];
    else
    {
        perror("A host address must be provided");
        exit(EXIT_FAILURE);
    }

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Filling server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert host address
    if (inet_pton(AF_INET, host, &servaddr.sin_addr) <= 0)
    {
        perror("Invalid host address");
        exit(EXIT_FAILURE);
    }

    // Run experiments
    runAndTimeExperiments(sockfd, servaddr);

    return 0;
}
