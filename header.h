//define macros

typedef struct item{
    char itemName[NAME_SIZE]; //define later
    unsigned int quantity;
    float amount;
}item;

typedef struct itemInfo{
    item *items;
    size_t itemCount; //about size_t
}itemInfo;

typedef struct block{
    time_t timestamp; //about time_t, human readable format
    char sender[NAME_SIZE];
    char receiver[NAME_SIZE];
    ItemInfo info; // Holds transaction data
    unsigned char prevHash[SHA256_DIGEST_LENGTH]; //define later
    unsigned char currHash[SHA256_DIGEST_LENGTH];
    unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
    uint32_t nonce; // Nonce for proof-of-work
    struct Block *next; //Pointer to next block
}block;