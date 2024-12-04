/* strtok() is destructive, meaning it changes the
 * input string by inserting null characters (\0).
 * If you need the original row after tokenization,
 * create a copy of it before calling strtok(). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"

// constant global variables to store login credentials
char NODE_PUBLIC_ID[PUBLIC_ID_SIZE] = "public-id-0001";
char NODE_PRIVATE_ID[PUBLIC_ID_SIZE] = "private-id-A1B2C3D4";

/* Initialises Blockchain */
void init_blockchain(Blockchain *chain)
{
	chain->head = NULL;
	chain->rear = NULL;
	return;
}

void initSHashTable(HashTable *table)
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		table->buckets[i] = NULL;
	}
}

/* takes parameters string filename and
 * read first line of header
 * stored second line for index, time, nonce, sender, reciever, itemcount in struct
 * stored item information like itemname, amount, quantity in struct
 * returns the struct BlockData read from file */
BlockData *ReadFile(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		perror("Error: Unable to open file. Please check if the file exists.\n\n");
		return NULL;
	}

	char row[MAX_LINE], *token, ch;
	int i, txnIndex = 0;

	// Initialize BlockData
	BlockData *blockData = (BlockData *)malloc(sizeof(BlockData));
	if (blockData == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed for blockData.\n");
		fclose(fp);
		return NULL;
	}
	blockData->NumOfTxn = 0;

	// Transaction numbers read
	i = 0;
	while ((ch = fgetc(fp)) != '\n' && ch != EOF)
	{
		if (i < MAX_LINE - 1)
		{
			row[i] = ch;
			i++;
		}
	}
	if (ch == EOF && i == 0)
	{
		fprintf(stderr, "Error: File does not contain sufficient data for block information.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	row[i] = '\0';
	token = strtok(row, ",");
	if (token == NULL || strcmp(token, "Number_of_Transactions") != 0)
	{
		fprintf(stderr, "Error: Missing or invalid header for transaction count.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	token = strtok(NULL, ",");
	if (token == NULL || (blockData->NumOfTxn = strtoul(token, NULL, 10)) == 0)
	{
		fprintf(stderr, "Error: Invalid or zero transaction count.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	blockData->NumOfTxn = strtoul(token, NULL, 10);
	if (blockData->NumOfTxn == 0)
	{
		fprintf(stderr, "Error: Item count must be greater than zero.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}

	blockData->info = (Info *)malloc(sizeof(Info) * blockData->NumOfTxn);

	if (blockData->info == NULL)
	{
		fprintf(stderr, "Memory allocation failed for transactions.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}

	// Ignore Title row
	i = 0;
	while ((ch = fgetc(fp)) != '\n' && ch != EOF)
	{
		if (i < MAX_LINE - 1)
		{
			row[i] = ch;
			i++;
		}
	}

	// Read each transaction
	while (fgets(row, MAX_LINE, fp) && txnIndex < blockData->NumOfTxn)
	{
		token = strtok(row, ",");
		if (token == NULL)
			break;
		strncpy(blockData->info[txnIndex].tnx, token, TNX_SIZE);

		token = strtok(NULL, ",");
		if (token == NULL)
			break;
		strncpy(blockData->info[txnIndex].senderID, token, PUBLIC_ID_SIZE);

		token = strtok(NULL, ",");
		if (token == NULL)
			break;
		strncpy(blockData->info[txnIndex].receiverID, token, PUBLIC_ID_SIZE);

		token = strtok(NULL, ",");
		if (token == NULL)
			break;
		blockData->info[txnIndex].amt = atof(token);

		txnIndex++;
	}

	fclose(fp);
	return blockData;
}

/* Adds block in blockchain
 * later add check for no repeated block*/
void AddBlock(Blockchain *chain, BlockData *blockData)
{
	block *newBlock = (block *)malloc(sizeof(block));
	if (newBlock == NULL)
	{
		perror("Error: Memory allocation failed.\n");
		return;
	}

	newBlock->data = *blockData;
	newBlock->next = NULL;

	// Set the timestamp to the current time
	newBlock->data.timestamp = time(NULL);

	// Add Block Nonce
	newBlock->data.nonce = 1234;

	updateMerkleRoot(blockData);
	memcpy(newBlock->data.merkleRoot, blockData->merkleRoot, SHA256_DIGEST_LENGTH);

	// Genesis block
	if (chain->head == NULL)
	{
		// Genesis Block's Previous Hash Will be 0000...
		// memset(blockData->prevHash, 0, SHA256_DIGEST_LENGTH);

		// Update newBlock's prevHash
		memset(newBlock->data.prevHash, 0, SHA256_DIGEST_LENGTH);
		newBlock->data.index = 1;

		Mineblock(blockData);
		memcpy(newBlock->data.currHash, blockData->currHash, SHA256_DIGEST_LENGTH);

		// newBlock->data.nonce = blockData->nonce;

		chain->head = newBlock;
		chain->rear = newBlock;
		printf("Block added successfully.\n");
		return;
	}
	memcpy(newBlock->data.prevHash, chain->rear->data.currHash, SHA256_DIGEST_LENGTH);
	newBlock->data.index = chain->rear->data.index + 1;

	// Generate Valid Hash of Block
	Mineblock(blockData);
	memcpy(newBlock->data.currHash, blockData->currHash, SHA256_DIGEST_LENGTH);

	newBlock->data.nonce = blockData->nonce;

	chain->rear->next = newBlock;
	chain->rear = newBlock;

	printf("Block added successfully.\n");
	return;
}

void printTimestamp(time_t timestamp)
{
	struct tm *tm_info = localtime(&timestamp);
	char buffer[26];
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	printf("%s", buffer);
	return;
}

/* Prints block based off index*/
void printBlock(Blockchain chain, int index)
{
	block *current = chain.head;
	while (current != NULL && index != current->data.index)
		current = current->next;

	if (current == NULL)
	{
		printf("\nBlock with index %d not found.\n", index);
		return;
	}
	// Retrieve the data for the block
	BlockData data = current->data;

	// Print Index
	printf("Block Index: %u\n", data.index);

	// Print time
	printf("Timestamp: ");
	printTimestamp(data.timestamp);
	printf("\n");

	// Print Transaction Details
	printf("Number of Transactions: %u\n", data.NumOfTxn);
	for (uint32_t i = 0; i < data.NumOfTxn; i++)
	{
		printf("\tTransaction %u:\n", i + 1);
		printf("\t  Transaction ID: %s\n", data.info[i].tnx);
		printf("\t  SenderID: %s\n", data.info[i].senderID);
		printf("\t  ReceiverID: %s\n", data.info[i].receiverID);
		printf("\t  Amount: %.2f\n", data.info[i].amt);
	}
	return;
}

/* Prints whole Blockchain*/
void printBlockchain(Blockchain chain)
{
	block *current = chain.head; // Start from the head of the blockchain

	printf("=========== Blockchain ===========\n\n");
	while (current != NULL)
	{
		BlockData data = current->data;

		// Print Index
		printf("Block Index: %u\n", data.index);

		// Print time
		printf("Timestamp: ");
		printTimestamp(data.timestamp);
		printf("\n");

		// Print Transaction Details
		printf("Number of Transactions: %u\n", data.NumOfTxn);
		for (uint32_t i = 0; i < data.NumOfTxn; i++)
		{
			printf("\tTransaction %u:\n", i + 1);
			printf("\t  Transaction ID: %s\n", data.info[i].tnx);
			printf("\t  SenderID: %s\n", data.info[i].senderID);
			printf("\t  ReceiverID: %s\n", data.info[i].receiverID);
			printf("\t  Amount: %.2f\n", data.info[i].amt);
		}

		// Print hash details
		printf("Previous Hash: ");
		printHash(data.prevHash);
		printf("\n");

		printf("Current Hash: ");
		printHash(data.currHash);
		printf("\n");

		printf("Merkle Root: ");
		printHash(data.merkleRoot);
		printf("\n");

		// Print nonce
		printf("Nonce: %u\n", data.nonce);
		printf("----------------------------------\n\n");

		current = current->next;
	}
	return;
}

/* Frees Blockchain after program is completed*/
void freeBlockchain(Blockchain *chain)
{
	block *current = chain->head;
	while (current != NULL)
	{
		block *nextBlock = current->next;

		// Free items in BlockData
		if (current->data.info != NULL)
			free(current->data.info);

		// Free the block itself
		free(current);
		current = nextBlock;
	}

	chain->head = NULL;
	chain->rear = NULL;
	return;
}

// MerkleTree
unsigned char **createLeaves(Info *info, size_t TxnCount)
{
	size_t i;
	unsigned char **hashes = (unsigned char **)malloc(TxnCount * sizeof(unsigned char *));
	for (i = 0; i < TxnCount; i++)
	{
		hashes[i] = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);

		// Create a data string for hashing
		char data[MAX_LINE];
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
			// If we have an odd number of hashes, just copy the last one
			newHashes[i] = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
			memcpy(newHashes[i], hashes[2 * i], SHA256_DIGEST_LENGTH);
		}
	}

	// Free the old hashes
	for (i = 0; i < numHashes; i++)
	{
		free(hashes[i]);
	}
	free(hashes);

	return newHashes;
}

// Main function to construct the Merkle tree and return the root hash
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

	// The last hash is the Merkle root
	unsigned char *merkleRoot = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
	memcpy(merkleRoot, hashes[0], SHA256_DIGEST_LENGTH);

	free(hashes[0]);
	free(hashes);

	return merkleRoot;
}

void updateMerkleRoot(BlockData *blockData)
{
	unsigned char *newMerkleRoot = constructMerkleTree(blockData->info, blockData->NumOfTxn);
	memcpy(blockData->merkleRoot, newMerkleRoot, SHA256_DIGEST_LENGTH);
	free(newMerkleRoot);
}

// hash function
uint32_t sbj4_hash(const char *publicID)
{
	// uint32_t hash = 0;
	// int i;

	// // Simple hash calculation
	// for (i = 9; publicID[i] != '\0'; i++)
	// {
	// 	hash = (hash * 37 + publicID[i]) % WALLET_TABLE_SIZE; // Prime multiplier 31
	// }
	int len = strlen(publicID);
	int v1 = publicID[len - 1] - '0', v2 = publicID[len - 2] - '0';
	return v2 * 10 + v1;
	// Return the hash value within table range
}

const char BLOCKCHAIN_NODES_PUBLIC_ID[][16] = {
	"public-id-0005", "public-id-0006", "public-id-0007", "public-id-0008",
	"public-id-0013", "public-id-0014", "public-id-0015", "public-id-0016",
	"public-id-0001", "public-id-0002", "public-id-0003", "public-id-0004",
	"public-id-0017", "public-id-0018", "public-id-0019", "public-id-0020",
	"public-id-0009", "public-id-0010", "public-id-0011", "public-id-0012"};

WalletStorage *WalletBank = NULL;

void InitWalletStorage()
{
	WalletBank = (WalletStorage *)malloc(sizeof(WalletStorage) * MAX_NODES_IN_BLOCKCHAIN);
	if (!WalletBank)
	{
		perror("Failed to allocate memory for WalletBank");
		exit(EXIT_FAILURE);
	}

	// Current nodes are 20
	for (int i = 0; i < 20; i++)
	{
		uint32_t hash = sbj4_hash(BLOCKCHAIN_NODES_PUBLIC_ID[i]);
		strcpy(WalletBank[hash].id, BLOCKCHAIN_NODES_PUBLIC_ID[i]);
		WalletBank[hash].balance = (10000 + rand() % 50000);
	}
}

void WalletPrint()
{
	for (int i = 0; i < 20; i++)
	{
		uint32_t hash = sbj4_hash(BLOCKCHAIN_NODES_PUBLIC_ID[i]);
		printf("public: %s balance: %f\n", WalletBank[hash].id, WalletBank[hash].balance);
	}
}

int isTxnBlockValid(Info *info, int n)
{
	for (int i = 0; i < n; i++)
	{
		char *senderID = info[i].senderID;
		float totalAmount = 0;

		for (int j = i; j < n; j++)
		{
			if (strcmp(senderID, info[j].senderID) == 0)
			{
				totalAmount += info[j].amt;
			}
		}

		unsigned int hash = sbj4_hash(senderID);
		if (WalletBank[hash].balance < totalAmount)
		{
			return 0;
		}

		while (i + 1 < n && strcmp(senderID, info[i + 1].senderID) == 0)
		{
			i++;
		}
	}
	return 1;
}

void update(char *senderID, char *receiverID, float amt)
{
	unsigned int senderHash = sbj4_hash(senderID);
	unsigned int receiverHash = sbj4_hash(receiverID);
	WalletBank[senderHash].balance -= amt;	 // Deduct from sender
	WalletBank[receiverHash].balance += amt; // add to receiver
	return;
}

void updateBalance(Info *info, int n)
{
	for (int i = 0; i < n; i++)
	{
		update(info[i].senderID, info[i].receiverID, info[i].amt);
	}
	return;
}

void loopupTable(HashTable table)
{
	printf("Transaction IDs are:\n");
	int i = 0, count = 0;
	HashNode *p;
	while (i < TABLE_SIZE)
	{
		p = table.buckets[i];
		while (p)
		{
			printf("%s\t\t", p->data->tnx);
			count++;
			p = p->next;
		}
		i++;
	}
	if (count == 0)
		printf("No transactions done yet.\n");
	return;
}

void ViewAccountDetails()
{
	uint32_t hash = sbj4_hash(NODE_PUBLIC_ID);
	printf("Public ID: [%s]\n", NODE_PUBLIC_ID);
	printf("Private ID: [%s]\n", NODE_PRIVATE_ID);

	if (!WalletBank)
	{
		printf("Wallet Balance: 0\n");
	}
	else
	{
		printf("Wallet Balance: %f\n", WalletBank[hash].balance);
	}
}

int isAuthenticated()
{
	char private_id[PUBLIC_ID_SIZE];
	char public_id[PUBLIC_ID_SIZE];

	printf("Enter your Public ID: ");
	scanf("%s", public_id);

	if (strcmp(public_id, NODE_PUBLIC_ID) != 0)
	{
		printf("Incorrect credentials. Try again.\n");
		return 0;
	}

	printf("Enter your Private ID: ");
	scanf("%s", private_id);

	if (strcmp(private_id, NODE_PRIVATE_ID) != 0)
	{
		printf("Incorrect credentials. Try again.\n");
		return 0;
	}
	return 1;
}

// your profile dashboard to view transactions data
void CreateProfileDashboard(HashTable table)
{
	int choice;
	printf("Enter your choice: ");
	scanf("%d", &choice);

	switch (choice)
	{
	case 1:
		isAuthenticated() ? loopupTable(table) : printf("Incorrect id..\n");
		break;

	case 2:
		isAuthenticated() ? ViewAccountDetails() : printf("Incorrect id..\n");

	default:
		break;
	}
	return;
}

// Mining Function

/* Logic :
	1. Mining function will generate the valid hash for the given block and change the currhash field of blockData
	2. 1st converting the iteration number i into hex string and consider this string as the nonce.
	3. Then calculating the hash by using calculateHash function.
	4. If Hash is valid then mining is completed => copy the hash into blockData->hash and return.
	5 else increment iteration number and step 2.
*/

void Mineblock(BlockData *blockData)
{
	unsigned char *hash;

	for (int i = 0;; i++)
	{
		// char hex[64];
		// sprintf(hex, "%x", i);
		// printf("%s ", hex);
		// strcpy(blockData->nonce, hex);
		blockData->nonce = i;
		hash = calculateHashForBlock(blockData);

		if (isHashValid(hash))
		{
			printf("Hash is Valid\n");
			printHash(hash);
			printf("%d ", i);
			memcpy(blockData->currHash, hash, SHA256_DIGEST_LENGTH);
			break;
		}
		else
		{
			continue;
		}
	}
	free(hash);
	return;
}

// Checking the hash is valid or not according to difficulty/
int isHashValid(unsigned char *hash)
{
	char prefix[DIFFICULTY + 1];
	for (int i = 0; i < DIFFICULTY; i++)
	{
		prefix[i] = 0;
	}
	prefix[DIFFICULTY] = '\0';

	if (strncmp((char *)hash, prefix, DIFFICULTY) == 0)
	{
		return 1;
	}
	else
		return 0;
}

/*Calculating hash:
	1. first concatinating in
	dex, timestamp, prevhash, merkleroot, nonce.
	2. generate the hash using sha256 and return that string.
	##note: memory is malloced in sha256, have to free that memory ==> freed in Mineblock   where to free hash??*/

unsigned char *calculateHashForBlock(BlockData *blockData)
{
	char record[256];
	snprintf(record, sizeof(record), "%u%ld%s%s%u", blockData->index, blockData->timestamp, blockData->prevHash, blockData->merkleRoot, blockData->nonce);
	unsigned char *outputhash = (unsigned char *)malloc(sizeof(unsigned char) * SHA256_DIGEST_LENGTH);
	computeSHA256((const char *)record, outputhash);
	return outputhash;
}

// to validate the blockchain
void isBlockChainValid(Blockchain B)
{
	block *p, *q;
	p = B.head;

	if (p == NULL)
	{
		printf("Empty BlockChain\n");
		return;
	}

	unsigned char *hash;
	// for genesis block check index == 1 or not
	if (p->data.index == 1)
	{
		// check the prevHash is equal to zero or not
		if (strspn((char *)p->data.prevHash, "0") != strlen((char *)p->data.prevHash))
		{
			printf("Corruption found in genesis block\n");
		}
	}
	else
	{
		printf("Corruption found in genesis block\n");
	}
	// checking the currHash of genesis block
	hash = calculateHashForBlock(&(p->data));
	if (strcmp((char *)hash, (char *)p->data.currHash) != 0)
	{
		free(hash);
		printf("Corruption found in block %d\n", p->data.index);
	}

	q = p; // q as previous block and p as current block
	p = p->next;
	while (p)
	{
		// Check if the index of the p is exactly 1 more than the q's index
		if (q->data.index + 1 != p->data.index)
		{
			printf("Corruption found in block %d\n", p->data.index);
		}

		// Check if the hash of the previous block matches the previous hash of the current block
		if (strcmp((char *)q->data.currHash, (char *)p->data.prevHash) != 0)
		{
			printf("Corruption found in block %d\n", p->data.index);
		}

		// checking the current hash is valid or not
		hash = calculateHashForBlock(&(p->data));
		if (strcmp((char *)hash, (char *)p->data.currHash) != 0)
		{
			printf("Corruption found in block %d\n", p->data.index);
		}
		free(hash);
		q = p;
		p = p->next;
	}

	printf("BlockChain Verification Successful\n");
	return;
}

// Merge Sort
// Function to merge two halves of the Info array
void merge(Info *info, int left, int mid, int right)
{
	int n1 = mid - left + 1;
	int n2 = right - mid;

	// Temporary arrays to hold data
	Info *leftArray = (Info *)malloc(n1 * sizeof(Info));
	Info *rightArray = (Info *)malloc(n2 * sizeof(Info));

	// Copy data to temporary arrays
	for (int i = 0; i < n1; i++)
		leftArray[i] = info[left + i];
	for (int i = 0; i < n2; i++)
		rightArray[i] = info[mid + 1 + i];

	// Merge the temporary arrays back into info[left..right]
	int i = 0, j = 0, k = left;
	while (i < n1 && j < n2)
	{
		// Compare sender IDs
		if (strcmp(leftArray[i].senderID, rightArray[j].senderID) <= 0)
		{
			info[k] = leftArray[i];
			i++;
		}
		else
		{
			info[k] = rightArray[j];
			j++;
		}
		k++;
	}

	// Copy remaining elements of leftArray, if any
	while (i < n1)
	{
		info[k] = leftArray[i];
		i++;
		k++;
	}

	// Copy remaining elements of rightArray, if any
	while (j < n2)
	{
		info[k] = rightArray[j];
		j++;
		k++;
	}

	// Free temporary arrays
	free(leftArray);
	free(rightArray);
}

// Function to implement merge sort on Info array
void mergeSort(Info *info, int left, int right)
{
	if (left < right)
	{
		// Calculate the midpoint
		int mid = left + (right - left) / 2;

		// Recursively sort the first and second halves
		mergeSort(info, left, mid);
		mergeSort(info, mid + 1, right);

		// Merge the sorted halves
		merge(info, left, mid, right);
	}
}

// Searchs
int findFirstIndex(Info transactions[], int size, const char *senderID)
{
	int left = 0, right = size - 1;
	int result = -1; // To store the first occurrence
	while (left <= right)
	{
		int mid = left + (right - left) / 2;
		if (strcmp(transactions[mid].senderID, senderID) == 0)
		{
			result = mid;	 // Update result
			right = mid - 1; // Continue searching in the left half
		}
		else if (strcmp(transactions[mid].senderID, senderID) < 0)
		{
			left = mid + 1; // Search in the right half
		}
		else
		{
			right = mid - 1; // Search in the left half
		}
	}
	return result;
}

void insertInSHashTable(int index, int numTxn, Info transactions[], HashTable *table)
{
	while (index < numTxn && strcmp(transactions[index].senderID, NODE_PUBLIC_ID) == 0)
	{
		insertTxnInfo(table, transactions, index);
		index++;
	}
	return;
}

void insertTxnInfo(HashTable *table, Info *info, int i)
{
	unsigned int index = generateSHash(info[i].tnx);
	HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
	if (!newNode)
	{
		perror("Failed to generate allocate memory.\n");
		return;
	}

	newNode->data = &info[i];
	newNode->next = table->buckets[index];
	table->buckets[index] = newNode;
	return;
}

unsigned int generateSHash(const char *txnID)
{
	unsigned long hash = 0xcbf29ce484222325;	   // Large prime initial value (FNV offset basis)
	unsigned long primeMultiplier = 0x100000001b3; // Prime multiplier (FNV prime)
	int i;

	for (i = 4; txnID[i] != '\0' && i < TNX_SIZE; i++)
	{
		unsigned char c;
		if (isdigit(txnID[i]))
		{
			c = (unsigned char)(txnID[i] - '0');
		}
		else if (isalpha(txnID[i]))
		{
			c = (unsigned char)(toupper(txnID[i]) - 'A' + 10);
		}
		else
		{
			continue; // Skip any non-alphanumeric characters
		}

		// Update the hash with a mix of current character and previous hash
		hash = hash ^ c;
		hash = (hash * primeMultiplier) % TABLE_SIZE; // Apply modulus to fit table size
	}

	// Final mixing step for better bit diffusion
	hash ^= (hash >> 33);
	hash *= primeMultiplier;
	hash ^= (hash >> 33);

	hash = hash % TABLE_SIZE; // Ensure hash is within table range

	return (unsigned int)hash;
}

Info *searchSHashTable(HashTable *table, const char *txnID)
{
	unsigned int index = generateSHash(txnID);
	HashNode *current = table->buckets[index];

	while (current)
	{
		printf("%s\n", current->data->tnx);
		printf("%s\n", txnID);
		if (strcmp(current->data->tnx, txnID) == 0)
		{
			printf("yes\n");
			return current->data;
		}
		current = current->next;
	}
	return NULL;
}

void printTransactionInfo(Info info)
{
	printf("Here are detials:\n");
	printf("TransactionID: %s\n", info.tnx);
	printf("SenderID: %s\n", info.senderID);
	printf("TransactionID: %s\n", info.receiverID);
	printf("TransactionID: %.2f\n", info.amt);
	return;
}