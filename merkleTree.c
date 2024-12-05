#include"header.h"

// Merkle Root Generation
unsigned char **createLeaves(Info *info, size_t TxnCount)
{
	size_t i;
	unsigned char **hashes = (unsigned char **)malloc(TxnCount * sizeof(unsigned char *));
    
	for (i = 0; i < TxnCount; i++)
	{
		hashes[i] = (unsigned char *)malloc(SHA256_DIGEST_LENGTH); 
        char data[MAX_LINE] = "";
		snprintf(data, MAX_LINE, "%s %s %s %.2f",
				 info[i].tnx, info[i].senderID, info[i].receiverID, info[i].amt);
		computeSHA256(data, hashes[i]);
	}
	return hashes;
}

// Function to combine two SHA256 hashes into one
unsigned char *combineHashes(unsigned char *hash1, unsigned char *hash2)
{
	unsigned char combined[2 * SHA256_DIGEST_LENGTH];
	memcpy(combined, hash1, SHA256_DIGEST_LENGTH);
	memcpy(combined + SHA256_DIGEST_LENGTH, hash2, SHA256_DIGEST_LENGTH);

	unsigned char *newHash = (unsigned char *)malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
	computeSHA256((const char *)combined, newHash);
	return newHash;
}

// Function to build the parent hashes from the current level of hashes
unsigned char **buildParentHashes(unsigned char **hashes, size_t numHashes, size_t *newHashCount)
{
	size_t i;
	*newHashCount = (numHashes + 1) / 2;
	unsigned char **newHashes = (unsigned char **)malloc(*newHashCount * sizeof(unsigned char *));

	for (i = 0; i < *newHashCount; i++)
	{
		if (2 * i + 1 < numHashes)
		{
			newHashes[i] = combineHashes(hashes[2 * i], hashes[2 * i + 1]);
		}
		else
		{
			newHashes[i] = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
			memcpy(newHashes[i], hashes[2 * i], SHA256_DIGEST_LENGTH);
		}
	}

	for (i = 0; i < numHashes; i++)
	{
		free(hashes[i]);
	}
	free(hashes);

	return newHashes;
}

/* Main function to construct the Merkle tree and return the root hash */
unsigned char *constructMerkleTree(Info *info, size_t TxnCount)
{
	if (TxnCount == 0)
		return NULL;

	unsigned char **hashes = createLeaves(info, TxnCount);
	size_t numHashes = TxnCount;
	size_t newHashCount;

	while (numHashes > 1)
	{
		hashes = buildParentHashes(hashes, numHashes, &newHashCount);
		numHashes = newHashCount;
	}

	unsigned char *merkleRoot = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
	memcpy(merkleRoot, hashes[0], SHA256_DIGEST_LENGTH);

	free(hashes[0]);
	free(hashes);

	return merkleRoot;
}

/* Updates merkleRoot in blockData*/
void updateMerkleRoot(BlockData *blockData)
{
	unsigned char *newMerkleRoot = constructMerkleTree(blockData->info, blockData->NumOfTxn);
	memcpy(blockData->merkleRoot, newMerkleRoot, SHA256_DIGEST_LENGTH);
	free(newMerkleRoot);
}