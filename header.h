#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME_SIZE 16
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32
#define PUBLIC_ID_SIZE 32
#define MAX_NODES_IN_BLOCKCHAIN 30
#define TNX_SIZE 17
#define TABLE_SIZE 101
#define WALLET_TABLE_SIZE 31
#define DIFFICULTY 3
#define NUM_USERS 20

#include "peer.h"


// Structure of Blockchain
typedef struct Info
{
    char tnx[TNX_SIZE];
    char senderID[PUBLIC_ID_SIZE];
    char receiverID[PUBLIC_ID_SIZE];
    float amt;

}Info;

typedef struct blockData
{
    uint32_t index;
    time_t timestamp;                               
    Info *info;                                      
    uint32_t NumOfTxn;
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
    uint32_t nonce;

} BlockData;

typedef struct block
{
    BlockData data;
    struct block *next;

}block;

typedef struct
{
    block *head;
    block *rear;

}Blockchain;


//Structure for HashTable
typedef struct HashNode
{
    Info *data;
    struct HashNode *next;

}HashNode;

typedef struct HashTable
{
    HashNode *buckets[TABLE_SIZE];
}HashTable;


typedef struct WalletStorage
{
    char id[PUBLIC_ID_SIZE];
    float balance;

}WalletStorage;

// FUNCTION PROTOTYPES
// Block Add Edit Print
void init_blockchain(Blockchain *chain);
void init_BlockData(BlockData *blockdata);
BlockData *ReadFile(char *filename);
void AddBlock(Blockchain *chain, BlockData *blockData);
void isBlockChainValid(Blockchain B);
void printBlockchain(Blockchain chain);
void printBlock(Blockchain chain, int index);
void freeBlockchain(Blockchain *chain);

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
int isTxnBlockValid(Info *info, int n);
void update(char* senderID, char* receiverID, float amt);
void updateBalance(Info* info, int n);

// dummy variable
void WalletPrint();

// your profile
void ProfileMenu();
void CreateProfileDashboard();
int isAuthenticated();
void loopupTable(HashTable table);

//Mining function
void Mineblock(BlockData *blockData);
int isHashValid(unsigned char *hash);
unsigned char *calculateHashForBlock(BlockData *blockData);

// Merge Sort
void mergeSort(Info *info, int left, int right);
void merge(Info *info, int left, int mid, int right);

//Search userID
int findFirstIndex(Info transactions[], int size, const char *senderID);

//Insert in HashTable
void insertInSHashTable(int index, int numTxn, Info transactions[], HashTable *table);
void insertInRHashTable(int numTxn, Info transactions[], HashTable *table);

//Search HashTable
void initSHashTable(HashTable *table);
unsigned int generateSHash(const char *txnID);
void insertTxnInfo(HashTable *table, Info *info, int index);
Info *searchSHashTable(HashTable *table, const char *txnID);
void freeSHashTable(HashTable *table);
void printTransactionInfo(Info info);

// Helper functions
void printTimestamp(time_t timestamp);