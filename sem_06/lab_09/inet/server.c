#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

#include "constants.h"
 
int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    int connects[MAX_COUNT_SOCK] = {0};

    int clen;
    struct sockaddr_in client_addr;
    
    fd_set set;
    int sock, max_sock;
    
    printf("htons(PORT) = %d\n", PORT);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    max_sock = sock;
    
    if (socket < 0)
    {
        printf("socket() failed: %d\n", errno);
        return ERROR_CREATE_SOCKET;
    }
    
    struct sockaddr_in serv_addr =
    {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("bind() failed: %d\n", errno);
        return ERROR_BIND_SOCKET;
    }
    

    if (listen(sock, 1) < 0)
    {
        printf("listen() failed: %d\n", errno);
        return ERROR_LISTEN_SOCKET;
    }
    
    printf("---Server started---\n");
    while (TRUE)
    {
        FD_ZERO(&set);
        FD_SET(sock, &set);
        max_sock = sock; 
        for (int i = 0; i < MAX_COUNT_SOCK; i++)
        {
            if (connects[i] > 0)
            {
                FD_SET(connects[i], &set);
                max_sock = connects[i] > max_sock ? connects[i] : max_sock;
            }
        }

        int retval = select(max_sock + 1, &set, NULL, NULL, NULL);
        if (retval < 0)
        {
            printf("select() failed: %d\n", errno);
            return ERROR_SELECT_SOCKET;
        }
        if (FD_ISSET(sock, &set))
        {
            printf("New connection.\n");
            int newsock = accept(sock, NULL, NULL);
            if (newsock < 0)
            {
                printf("accept() failed: %d\n", errno);
                return ERROR_ACCEPT_SOCKET;
            }
            int flag = 1;
            for (int i = 0; i < MAX_COUNT_SOCK && flag; i++)
            {
                if (connects[i] == 0)
                {
                    connects[i] = newsock;
                    printf("Client %d\n", i);
                    flag = 0;
                }
            }
            if (flag)
            {
                printf("Couldn't create new connection.\n");
            }
        }

        for (int i = 0; i < MAX_COUNT_SOCK; i++)
        {
            if (connects[i] && FD_ISSET(connects[i], &set))
            {
                char buf[MAX_LEN_BUF];
                int rv = recvfrom(connects[i], buf, sizeof(buf), 0, NULL, NULL);
                if (rv == 0)
                {
                    printf("Client %d disconnected\n", i);
                    close(connects[i]);
                    connects[i] = 0;
                }
                else
                {
                    printf("Client %d: %s\n", i, buf);
                }
                send(connects[i], buf, rv, 0);
            }
        }
    }
    close(sock);
    return OK;
}