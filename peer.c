#include "header.h"
#define BLOCK_NAME_SIZE 20

// Struct to store arguments for the receive thread
ThreadArguments *StoreReceiveThreadArgs(int server_fd, int blockIndx)
{
    ThreadArguments *arg = (ThreadArguments *)malloc(sizeof(ThreadArguments));
    arg->server_fd = server_fd;
    arg->blockIndx = blockIndx;
    return arg;
}

// Main function to establish a P2P network connection
int P2P_NetworkConnection(int blockIndx)
{
    int PORT = 8080; // Port for incoming connections
    int server_fd;
    struct sockaddr_in address;

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable reuse of the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Start a thread to handle incoming connections
    pthread_t tid;
    ThreadArguments *thread_arg = StoreReceiveThreadArgs(server_fd, blockIndx);
    pthread_create(&tid, NULL, receive_thread, thread_arg);

    // Display user menu for sending files or quitting
    printf("\n*****At any point, press the following:*****\n");
    printf("1. Send file\n");
    printf("0. Quit\n");

    int choice;
    do
    {
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            // Handle file sending
            sending(blockIndx);
            break;
        case 0:
            // Exit the program
            printf("\nExiting\n");
            break;
        default:
            // Handle invalid input
            printf("\nInvalid choice\n");
        }
    } while (choice != 0);

    // Clean up and close the server socket
    close(server_fd);
    return 0;
}

// Function to send a file over a socket
void send_file(int socketFD, char blockname[])
{
    FILE *fp = fopen(blockname, "r");
    if (fp == NULL)
    {
        perror("Error in reading file");
        exit(EXIT_FAILURE);
    }

    char data[BUFFER_SIZE] = {0};

    // Read file data and send over socket
    while (fgets(data, BUFFER_SIZE, fp) != NULL)
    {
        if (send(socketFD, data, sizeof(data), 0) == -1)
        {
            perror("Error in sending data");
            exit(EXIT_FAILURE);
        }
        bzero(data, BUFFER_SIZE); // Clear buffer after sending
    }
    fclose(fp); // Close the file
}

// Function to send files to multiple peers
void sending(char *blockname)
{
    int PORT_LIST[] = {8081, 8082}; // List of ports to send the file
    int ports_len = sizeof(PORT_LIST) / sizeof(PORT_LIST[0]);
    char ip_address[INET_ADDRSTRLEN] = "127.0.0.1"; // Localhost IP

    for (int i = 0; i < ports_len; i++)
    {
        int sock;
        struct sockaddr_in serv_addr;

        // Create a socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket creation error");
            return;
        }

        // Configure server address
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT_LIST[i]);

        // Convert IP address to binary form
        if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0)
        {
            perror("Invalid address/Address not supported");
            return;
        }

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("Connection failed");
            return;
        }

        // Send the file
        send_file(sock, blockname);
        close(sock); // Close the socket after sending
    }
}

// Thread function to handle incoming connections
void *receive_thread(void *args)
{
    ThreadArguments *arg = (ThreadArguments *)args;
    int server_fd = arg->server_fd;
    int blockIndx = arg->blockIndx;

    // Start receiving files
    receiving(server_fd, blockIndx);
    pthread_exit(NULL);
}

// Function to write received data to a file
void write_file(int client_socket_fd, int blockIndx)
{
    char ServerBlock[BLOCK_NAME_SIZE];

    // Updtae Block Name in server side
    snprintf(ServerBlock, BLOCK_NAME_SIZE, "Block%d.csv", blockIndx);

    printf("%s ", ServerBlock);

    FILE *fp = fopen(ServerBlock, "w");
    if (fp == NULL)
    {
        perror("Error in creating file");
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Receive data and write to file
    while ((bytesReceived = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        buffer[bytesReceived] = '\0';
        fputs(buffer, fp);
    }

    if (bytesReceived < 0)
    {
        perror("Error receiving data");
    }
    printf("\n%s received successfully\n", ServerBlock);
    close(client_socket_fd);
    fclose(fp);
}

// Function to handle incoming connections and receive files
void receiving(int server_fd, int blockIndx)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set current_sockets, ready_sockets;

    FD_ZERO(&current_sockets);           // Clear socket set
    FD_SET(server_fd, &current_sockets); // Add server socket to set

    while (1)
    {
        ready_sockets = current_sockets;

        // Monitor sockets for activity
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_fd)
                {
                    // Accept a new connection
                    int client_socket;
                    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                    {
                        perror("Accept failed");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(client_socket, &current_sockets); // Add new socket to set
                }
                else
                {
                    // Handle data from a connected client
                    write_file(i, blockIndx);
                    close(i);                    // Close client socket after handling
                    FD_CLR(i, &current_sockets); // Remove socket from set
                }
            }
        }
    }
}
