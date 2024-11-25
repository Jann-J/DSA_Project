#include <stdio.h>
#include <stdint.h>
#include <time.h>
#define NAME_SIZE 50
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32
#define PUBLIC_ID_SIZE 32
#define TOTAL_NODES_IN_BLOCKCHAIN 20

// networking
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#define BUFFER_SIZE 1024

typedef struct item
{
    char itemName[NAME_SIZE]; // define later
    uint32_t quantity;
    float amount;
} item;

typedef struct itemInfo
{
    char sender[NAME_SIZE];
    char receiver[NAME_SIZE];
    item *items;
    size_t itemCount; // about size_t
} itemInfo;

typedef struct blockData
{
    uint32_t index;
    time_t timestamp;                             // about time_t, human readable format
    itemInfo info;                                // Holds transaction data
    unsigned char prevHash[SHA256_DIGEST_LENGTH]; // define later
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
    uint32_t nonce; // Nonce for proof-of-work
} BlockData;

typedef struct block
{
    BlockData data;
    struct block *next;
} block;

typedef struct
{
    block *head;
    block *rear;
} Blockchain;

// Networking
// Receiver Thread Argument Structure
typedef struct ThreadArguments
{
    int server_fd;
    char *blockname;
} ThreadArguments;

// Hash Table For Faster Transaction Validation

typedef struct txInfo
{
    char id[PUBLIC_ID_SIZE];
    char sender[NAME_SIZE];
    char receiver[NAME_SIZE];
    item *items;
    size_t itemCount; // about size_t
} txInfo;

typedef struct WalletStorage
{
    char *id;
    int balance;
}WalletStorage[TOTAL_NODES_IN_BLOCKCHAIN];

// FUNCTION PROTOTYPES
// Block Add Edit Print
void init_blockchain(Blockchain *chain);
void init_BlockData(BlockData *blockdata);
BlockData *ReadFile(char *filename);
void AddBlock(Blockchain *chain, BlockData *blockData);
void printBlockchain(Blockchain chain);
void addInfoToBlock(BlockData *blockData);
void freeBlockchain(Blockchain *chain);
void printBlock(Blockchain chain, int index); // based off index
void EditBlock(Blockchain chain, char *filename);

// SHA256
void computeSHA256(const char *data, unsigned char *hash);
void printHash(unsigned char *hash, long unsigned int);

// Merkle Root Generation
unsigned char **createLeaves(item *items, size_t itemCount);
unsigned char *combineHashes(unsigned char *hash1, unsigned char *hash2);
unsigned char **buildParentHashes(unsigned char **hashes, size_t numHashes, size_t *newHashCount);

// Networking
int P2P_NetworkConnection(char *blockname);
void sending();
void receiving(int server_fd, char *blockname);
void *receive_thread(void *server_fd);

// hashtable for Transaction Validation
WalletStorage* CreatesNodesWithRandomBalance();
txInfo *InputTransactionData();
int ValidateTransactionData(txInfo* newtx, WalletStorage *WalletBank);