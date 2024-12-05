#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define WALLET_TABLE_SIZE 101

uint32_t sbj4_hash(const char *publicID)
{
    uint32_t hash = 0;
    int prime = 31; // Prime multiplier for better distribution
    int len = strlen(publicID);

    // Mix characters of the string into the hash
    for (int i = 0; i < len; i++)
    {
        hash = (hash * prime + publicID[i]) % WALLET_TABLE_SIZE;
    }

    // Return the hash value within table range
    return hash;
}

/* Array of Nodes in Blockchain*/
const char BLOCKCHAIN_NODES_PUBLIC_ID[][16] = {
    "public-id-0005", "public-id-0006", "public-id-0007", "public-id-0008",
    "public-id-0013", "public-id-0014", "public-id-0015", "public-id-0016",
    "public-id-0001", "public-id-0002", "public-id-0003", "public-id-0004",
    "public-id-0017", "public-id-0018", "public-id-0019", "public-id-0020",
    "public-id-0009", "public-id-0010", "public-id-0011", "public-id-0012"};

int main()
{

    for (int i = 0; i < 20; i++)
    {
        printf("%d ", sbj4_hash(BLOCKCHAIN_NODES_PUBLIC_ID[i]));
    }
    

    return 0;
}