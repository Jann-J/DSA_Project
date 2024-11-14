#include <stdint.h>
#include <time.h>
#define NAME_SIZE 50
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32

typedef struct item{
    char itemName[NAME_SIZE]; //define later
    uint32_t quantity;
    float amount;
}item;

typedef struct itemInfo{
    char sender[NAME_SIZE];
    char receiver[NAME_SIZE];
    item *items;
    size_t itemCount;//about size_t
}itemInfo;

typedef struct blockData{
    uint32_t index;
    time_t timestamp; //about time_t, human readable format
    itemInfo info; // Holds transaction data
    unsigned char prevHash[SHA256_DIGEST_LENGTH]; //define later
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
    uint32_t nonce; // Nonce for proof-of-work
}BlockData;

typedef struct block {
    BlockData data;
    struct block *next;
}block;

typedef struct {
	block *head;
	block *rear;
}Blockchain;

//FUNCTION PROTOTYPES
void init_blockchain(Blockchain *chain);
void init_BlockData(BlockData *blockdata);
BlockData *ReadFile(char *filename);
void AddBlock(Blockchain *chain, BlockData *blockData);
void printBlockchain(Blockchain chain);
void addInfoToBlock(BlockData *blockData);
void freeBlockchain(Blockchain *chain);
void printBlock(Blockchain chain, int index);//based off index


unsigned char *create_sha256(const unsigned char str[]);
