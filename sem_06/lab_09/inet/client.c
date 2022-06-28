#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
 
#include "constants.h"
 
int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    struct sockaddr_in serv_addr =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };
    char buf[MAX_LEN_BUF];
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        perror("socket() failed");
        return ERROR_CREATE_SOCKET;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("connect() failed: %d", errno);
        return ERROR_BIND_SOCKET;
    }
    snprintf(buf, BUFSIZ, "[process %d]: aaaaa", getpid());
    if (sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("sendto() failed");
        return ERROR_SENDTO_SOCKET;
    }
    printf("Message sended!\n");
    recv(sock, buf, sizeof(buf), 0);
    printf(buf);
    close(sock);
    return OK;
}
 