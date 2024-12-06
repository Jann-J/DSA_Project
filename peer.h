#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>    // For data types like `socklen_t`
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <linux/socket.h>

#define BUFFER_SIZE 1024

typedef struct ThreadArguments
{
    int server_fd;
    int blockIndx;
} ThreadArguments;

// Networking
int P2P_NetworkConnection(int blockIndx);
void sending( int blockIndx);
void receiving(int server_fd, int blockIndx);
void *receive_thread(void *server_fd);