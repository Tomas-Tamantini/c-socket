#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 54321
#define MAX_LINE 256

int main(int argc, char *argv[])
{
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;

    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        fprintf(stderr, "usage: simplex-talk host\n");
        exit(1);
    }

    /* Monta a estrutura de dados do endereço */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERVER_PORT);

    /* Traduz endereço do host para binário */
    if (inet_pton(AF_INET, host, &sin.sin_addr) <= 0)
    {
        printf("\nsimplex-talk: invalid address \n");
        return -1;
    }

    /* Abertura ativa */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("simplex-talk: socket");
        exit(1);
    }
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("simplex-talk: connect");
        close(s);
        exit(1);
    }

    /* Loop principal: obtém e envia linhas de texto */
    while (fgets(buf, sizeof(buf), stdin))
    {
        buf[MAX_LINE - 1] = '\0';
        len = strlen(buf) + 1;
        send(s, buf, len, 0);
    }
}