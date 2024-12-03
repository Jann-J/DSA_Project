#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#include<stdio.h>

#define BUFFER_SIZE 1024

typedef struct ThreadArguments
{
    int server_fd;
    char *blockname;
} ThreadArguments;

// Networking
int P2P_NetworkConnection(char *blockname);
void sending();
void receiving(int server_fd, char *blockname);
void *receive_thread(void *server_fd);