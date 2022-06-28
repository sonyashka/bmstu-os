#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include "socket.h"

static int client_sfd;

void cleanup_socket(int sockfd) {
  close(sockfd);
  char full_path[BUF_SIZE] = {0};
  strcat(full_path, SOCKET_NAME);
  strcat(full_path, CLIENT_PREFIX);
  printf("cleanup fullname = %s\n", full_path);
  unlink(full_path);
}

void sighandler(int signum) {
  cleanup_socket(client_sfd);
  exit(0);
}

int main(int argc, char* argv[]) {
  int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("socket call error");
    return errno;
  }
  client_sfd = sfd;
  if (signal(SIGINT, sighandler) == SIG_ERR) {
    perror("signal call error");
    cleanup_socket(sfd);
    return errno;
  }
  
  struct sockaddr_un addr_client;
  addr_client.sun_family = AF_UNIX;
  
  char full_path_client[BUF_SIZE] = {0};
  strcat(full_path_client, SOCKET_NAME);
  strcat(full_path_client, CLIENT_PREFIX);

  strcpy(addr_client.sun_path, full_path_client);
  
  if (bind(sfd, (struct sockaddr*)&addr_client, sizeof(addr_client)) == -1) {
    perror("bind call error");
    cleanup_socket(sfd);
    return errno;
  }
 
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  char full_path[BUF_SIZE] = {0};
  strcat(full_path, SOCKET_NAME);
 
  strcpy(addr.sun_path, full_path);
  char msg[BUF_SIZE];

snprintf(msg, BUF_SIZE, "[process %d]: aaaaa", getpid());
  if (sendto(sfd, msg, strlen(msg), 0, (struct sockaddr*) &addr, sizeof(addr)) == -1) { 
    perror("sendto call error");
    return errno;
  } else {
    printf("Send accessed %s\n", addr.sun_path);
  }

  char servermsg[BUF_SIZE] = {0};    
  size_t bytes = recv(sfd, servermsg, sizeof(servermsg), 0);
  if (bytes < 0) {
    perror("recv failed");
    return errno;
  } else {
    printf("received message: '%s'\n", servermsg);
  }
  sleep(1000);
  cleanup_socket(sfd);
  return 0;
}