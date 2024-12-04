#include <stdio.h>
#include <stdint.h>
#include <time.h>
#define NAME_SIZE 50
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32
#define PUBLIC_ID_SIZE 32
#define MAX_NODES_IN_BLOCKCHAIN 30
#define TNX_SIZE 16
#define TABLE_SIZE 101
#define WALLET_TABLE_SIZE 31
#define DIFFICULTY 2

// networking
#include "peer.h"

// Structure of Blockchain
typedef struct Info{
    char tnx[TNX_SIZE];
    char senderID[PUBLIC_ID_SIZE];
    char receiverID[PUBLIC_ID_SIZE];
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

// UPDATED TRANSACTION STRUCTURE
typedef struct txInfo
{
    char id[PUBLIC_ID_SIZE];
    char sender[NAME_SIZE];
    char receiver[NAME_SIZE];
    uint32_t txBalance;
} txInfo;

typedef struct WalletStorage
{
    char id[PUBLIC_ID_SIZE];
    int balance;
}WalletStorage;

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
void printHash(unsigned char *hash);

// Merkle Root Generation
unsigned char **createLeaves(Info *info, size_t TxnCount);
unsigned char *combineHashes(unsigned char *hash1, unsigned char *hash2);
unsigned char **buildParentHashes(unsigned char **hashes, size_t numHashes, size_t *newHashCount);
unsigned char *constructMerkleTree(Info *info, size_t TxnCount);
void updateMerkleRoot(BlockData *blockData);

// hashtable for Transaction Validation
void InitWalletStorage();
int ValidateTransactionData(BlockData *data);
int isTxn_inblock_valid(Info **info, int n);

// create account
void CreateAccount();

// your profile
void ProfileMenu();
void CreateProfileDashboard();
int isAuthenticated();

//Mining function prototypes
void Mineblock(BlockData *blockData);
int isHashValid(unsigned char *hash);
unsigned char *calculateHashForBlock(BlockData *blockData);

//block chain validation
void isBlockChainValid(Blockchain B);

// Merge Sort
void mergeSort(Info *info, int left, int right);
void merge(Info *info, int left, int mid, int right);