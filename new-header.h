#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <openssl/evp.h>
#define NAME_SIZE 50
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32
#define TNX_SIZE 16
#define ID_SIZE 16
#define TABLE_SIZE 101
#define DIFFICULTY 2

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


//Structure of Blockchain
typedef struct Info{
    char tnx[TNX_SIZE];
    char senderID[ID_SIZE];
    char receiverID[ID_SIZE];
    float amt;
}Info;

typedef struct blockData{
    uint32_t index;
    time_t timestamp;                               // about time_t, human readable format
    Info *info;                                      // Holds transaction data
    uint32_t NumOfTxn;
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
    uint32_t nonce;                                 // Nonce for proof-of-work
} BlockData;

typedef struct block{
    BlockData data;
    struct block *next;
}block;

typedef struct{
    block *head;
    block *rear;
}Blockchain;

//Structure for HashTable
typedef struct HashNode{
    Info *data;
    struct HashNode *next;
}HashNode;

typedef struct HashTable{
    HashNode *buckets[TABLE_SIZE];
}HashTable;

// Networking
// Receiver Thread Argument Structure
typedef struct ThreadArguments{
    int server_fd;
    char *blockname;
} ThreadArguments;

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
unsigned char **createLeaves(Info *info, size_t TxnCount);
unsigned char *combineHashes(unsigned char *hash1, unsigned char *hash2);
unsigned char **buildParentHashes(unsigned char **hashes, size_t numHashes, size_t *newHashCount);
unsigned char *constructMerkleTree(Info *info, size_t TxnCount);
void updateMerkleRoot(BlockData *blockData);

// Networking
int P2P_NetworkConnection(char *blockname);
void sending();
void receiving(int server_fd, char *blockname);
void *receive_thread(void *server_fd);