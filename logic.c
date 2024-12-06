#include "header.h"

/* Constant global variables to store login credentials*/
char NODE_PUBLIC_ID[PUBLIC_ID_SIZE] = "public-id-0001";
char NODE_PRIVATE_ID[PUBLIC_ID_SIZE] = "private-id-A1B2C3D4";

/* Array of Nodes in Blockchain*/
const char BLOCKCHAIN_NODES_PUBLIC_ID[][16] = {
		"public-id-0005", "public-id-0006", "public-id-0007", "public-id-0008",
		"public-id-0013", "public-id-0014", "public-id-0015", "public-id-0016",
		"public-id-0001", "public-id-0002", "public-id-0003", "public-id-0004",
		"public-id-0017", "public-id-0018", "public-id-0019", "public-id-0020",
		"public-id-0009", "public-id-0010", "public-id-0011", "public-id-0012"
	};

/* Initial balance corresponding to the hardcoded public IDs */
float init_balance[20] = {
		45678.34, 23567.89, 58912.45, 30012.78, 48005.65,
		21987.44, 34899.12, 56000.50, 41023.89, 60000.00,
		22000.12, 45987.56, 32000.34, 58000.78, 26000.45,
		54000.23, 39000.56, 49999.99, 28000.67, 53000.45
	};

/* Initialises Blockchain */
void init_blockchain(Blockchain *chain)
{
	chain->head = NULL;
	chain->rear = NULL;
	return;
}

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

	newBlock->data.timestamp = time(NULL);							//make it constant

	newBlock->data.nonce = 1234;

	updateMerkleRoot(blockData);
	memcpy(newBlock->data.merkleRoot, blockData->merkleRoot, SHA256_DIGEST_LENGTH);

	if(!validateDup(*chain, newBlock->data.merkleRoot)){
		printf("Block already exists. Cannot be added again.\n");
		free(newBlock);
		return;
	}

	// Genesis block
	if (chain->head == NULL)
	{
		// Update newBlock's prevHash
		memset(newBlock->data.prevHash, 0, SHA256_DIGEST_LENGTH);
		newBlock->data.index = 1;

		printf("Mining...\n\n");
		Mineblock(blockData);
		printf("Mining Completed.\n\nBlock is Valid.\n");

		memcpy(newBlock->data.currHash, blockData->currHash, SHA256_DIGEST_LENGTH);

		// newBlock->data.nonce = blockData->nonce;
		chain->head = newBlock;
		chain->rear = newBlock;
		printf("Block added successfully.\n");
		return;
	}
	
	memcpy(newBlock->data.prevHash, chain->rear->data.currHash, SHA256_DIGEST_LENGTH);
	
	newBlock->data.index = chain->rear->data.index + 1;
	
	printf("Mining...\n\n");
	Mineblock(blockData);
	printf("Mining Completed.\n\nBlock is Valid.\n\n");
	
	memcpy(newBlock->data.currHash, blockData->currHash, SHA256_DIGEST_LENGTH);

	newBlock->data.nonce = blockData->nonce;

	chain->rear->next = newBlock;
	chain->rear = newBlock;

	printf("Block added successfully.\n");
	return;
}


/* Logic :
	1. Mining function will generate the valid hash for the 
		given block and change the currhash field of blockData
	2. Initializing iteration number as nonce of the blockdata
	3. Then calculating the hash by using calculateHash function.
	4. If Hash is valid then mining is completed => copy the hash 
		into blockData->hash and return.
	5 else increment iteration number and step 2.
*/
void Mineblock(BlockData *blockData)
{
	unsigned char *hash;

	for (int i = 0;; i++)
	{
		blockData->nonce = i;
		hash = calculateHashForBlock(blockData);

		if (isHashValid(hash))
		{
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
	
	for(int i = 0; i < DIFFICULTY; i++) {
		if(hash[i] == 0) {
			continue;
		}
		else {
			return 0;
		}
	}
	return 1;
}


int validateDup(Blockchain chain, unsigned char *merkleRoot){
	block *p = chain.head;
	while(p){
		if (memcmp(p->data.merkleRoot, merkleRoot, SHA256_DIGEST_LENGTH) == 0){
			return 0;
		}
		p = p->next;
	}
	return 1;
}


/*Calculating hash:
	1. first concatinating index, timestamp, prevhash, merkleroot, nonce.
	2. generate the hash using sha256 and return that string.
	##note: memory is malloced in sha256, have to free that memory ==> freed in Mineblock where to free hash??*/

unsigned char *calculateHashForBlock(BlockData *blockData)
{
	char record[256];
	snprintf(record, sizeof(record), "%u%ld%s%s%u", blockData->index,blockData->timestamp, blockData->prevHash, blockData->merkleRoot, blockData->nonce);
	unsigned char *outputhash = (unsigned char *)malloc(sizeof(unsigned char) * SHA256_DIGEST_LENGTH);
	computeSHA256((const char *)record, outputhash);
	return outputhash;
}


/* Prints block based on index*/
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
	BlockData data = current->data;

	printf("Block Index: %u\n", data.index);

	printf("Timestamp: ");
	printTimestamp(data.timestamp);
	printf("\n");

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


/* Prints the whole Blockchain*/
void printBlockchain(Blockchain chain)
{
	block *current = chain.head;

	printf("=========== Blockchain ===========\n\n");
	while (current != NULL)
	{
		BlockData data = current->data;

		printf("Block Index: %u\n", data.index);

		printf("Number of Transactions: %u\n", data.NumOfTxn);
		for (uint32_t i = 0; i < data.NumOfTxn; i++)
		{
			printf("\tTransaction %u:\n", i + 1);
			printf("\t  Transaction ID: %s\n", data.info[i].tnx);
			printf("\t  SenderID: %s\n", data.info[i].senderID);
			printf("\t  ReceiverID: %s\n", data.info[i].receiverID);
			printf("\t  Amount: %.2f\n", data.info[i].amt);
		}

		printf("Previous Hash: ");
		printHash(data.prevHash);
		printf("\n");

		printf("Current Hash: ");
		printHash(data.currHash);
		printf("\n");

		printf("Merkle Root: ");
		printHash(data.merkleRoot);
		printf("\n");

		printf("Nonce: %u\n", data.nonce);
		printf("----------------------------------\n\n");

		current = current->next;
	}
	return;
}



void initSHashTable(HashTable *table)
{
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		table->buckets[i] = NULL;
	}
}

/******************Wallet Functions********************/

uint32_t sbj4_hash(const char *publicID)
{
	
	int len = strlen(publicID);
	int v1 = publicID[len - 1] - '0', v2 = publicID[len - 2] - '0';
	return v2 * 10 + v1;
}

WalletStorage *WalletBank = NULL;

void InitWalletStorage()
{
	WalletBank = (WalletStorage *)malloc(sizeof(WalletStorage) * MAX_NODES_IN_BLOCKCHAIN);
	if (!WalletBank)
	{
		perror("Failed to allocate memory for WalletBank");
		exit(EXIT_FAILURE);
	}


	for (int i = 0; i < NUM_USERS; i++)
	{
		uint32_t hash = sbj4_hash(BLOCKCHAIN_NODES_PUBLIC_ID[i]);	
		strcpy(WalletBank[hash].id, BLOCKCHAIN_NODES_PUBLIC_ID[i]); 
		WalletBank[hash].balance = init_balance[i];					
	}
}


/*void WalletPrint()
{
	for (int i = 0; i < NUM_USERS; i++)
	{
		uint32_t hash = sbj4_hash(BLOCKCHAIN_NODES_PUBLIC_ID[i]); 
		printf("public: %s balance: %f\n", WalletBank[hash].id, WalletBank[hash].balance);
	}
}*/


// Validate transaction block based on sender balance
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


// Update balances for sender and receiver after a transaction
void update(char *senderID, char *receiverID, float amt)
{
	unsigned int senderHash = sbj4_hash(senderID);	   
	unsigned int receiverHash = sbj4_hash(receiverID); 
	WalletBank[senderHash].balance -= amt;			   
	WalletBank[receiverHash].balance += amt;		   
	return;
}

// Update wallet balances for all transactions in a block
void updateBalance(Info *info, int n)
{
	for (int i = 0; i < n; i++)
	{
		update(info[i].senderID, info[i].receiverID, info[i].amt); // Update balances for each transaction
	}
	return;
}



/****************** Search by Transactions ********************/

// Print all transaction IDs stored in the hash table
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

// View details of a user's account
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
		printf("Wallet Balance: %.2f\n", WalletBank[hash].balance);
	}
}

// Authenticate user by verifying public and private IDs
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

// Create a user profile dashboard to view transactions or account details
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
		break;

	default:
		break;
	}
	return;
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

	if (p->data.index == 1)
	{
		size_t zero_count = 0;
		for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    		if (p->data.prevHash[i] != 0) 
				break;
   	   		zero_count++;
		}
		if (zero_count != SHA256_DIGEST_LENGTH) {
    	printf("Corruption found in genesis block\n");
		}
	}
	else
	{
		printf("Corruption found blockchain\n");
	}

	q = p;
	p = p->next;
	while (p)
	{
		if (q->data.index + 1 != p->data.index)
		{
			printf("Corruption found in block %d\n", p->data.index);
		}

		if (strcmp((char *)q->data.currHash, (char *)p->data.prevHash) != 0)			//memcpy
		{
			printf("Corruption found in block %d\n", p->data.index);
		}
		q = p;
		p = p->next;
	}

	printf("BlockChain Verification Successful\n");
	return;
}

/****************** MERGE SORT ********************/
// Function to merge two halves of the Info array
void merge(Info *info, int left, int mid, int right)
{
	int n1 = mid - left + 1;
	int n2 = right - mid;

	
	Info *leftArray = (Info *)malloc(n1 * sizeof(Info));
	Info *rightArray = (Info *)malloc(n2 * sizeof(Info));

	for (int i = 0; i < n1; i++)
		leftArray[i] = info[left + i];
	for (int i = 0; i < n2; i++)
		rightArray[i] = info[mid + 1 + i];

	int i = 0, j = 0, k = left;
	while (i < n1 && j < n2)
	{

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

	while (i < n1)
	{
		info[k] = leftArray[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		info[k] = rightArray[j];
		j++;
		k++;
	}

	free(leftArray);
	free(rightArray);
}


// Function to implement merge sort on Info array
void mergeSort(Info *info, int left, int right)
{
	if (left < right)
	{
		int mid = left + (right - left) / 2;

		mergeSort(info, left, mid);
		mergeSort(info, mid + 1, right);

		merge(info, left, mid, right);
	}
}

// Searchs
int findFirstIndex(Info transactions[], int size, const char *senderID)
{
	int left = 0, right = size - 1;
	int result = -1;
	while (left <= right)
	{
		int mid = left + (right - left) / 2;
		if (strcmp(transactions[mid].senderID, senderID) == 0)
		{
			result = mid;	
			right = mid - 1;
		}
		else if (strcmp(transactions[mid].senderID, senderID) < 0)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
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

void insertInRHashTable(int numTxn, Info transactions[], HashTable *table){
	int index = 0;
	while(index < numTxn) {
		if(strcmp(transactions[index].receiverID, NODE_PUBLIC_ID) == 0)
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
	unsigned long hash = 0xcbf29ce484222325;	   
	unsigned long primeMultiplier = 0x100000001b3; 
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
			continue;
		}

		hash = hash ^ c;
		hash = (hash * primeMultiplier) % TABLE_SIZE; 
	}

	
	hash ^= (hash >> 33);
	hash *= primeMultiplier;
	hash ^= (hash >> 33);

	hash = hash % TABLE_SIZE; 
	return (unsigned int)hash;
}

Info *searchSHashTable(HashTable *table, const char *txnID)
{
	unsigned int index = generateSHash(txnID);
	HashNode *current = table->buckets[index];

	while (current)
	{
		if (strcmp(current->data->tnx, txnID) == 0)
		{
			return current->data;
		}
		current = current->next;
	}
	return NULL;
}

void printTransactionInfo(Info info)
{
	printf("\nHere are the Transaction detials:\n");
	printf("TransactionID: %s\n", info.tnx);
	printf("SenderID: %s\n", info.senderID);
	printf("ReceiverID: %s\n", info.receiverID);
	printf("Amount: %.2f\n", info.amt);
	return;
}

/* Frees Blockchain after program is completed*/
void freeBlockchain(Blockchain *chain)
{
	block *current = chain->head;
	while (current != NULL)
	{
		block *nextBlock = current->next;

		
		if (current->data.info != NULL)
			free(current->data.info);

		
		free(current);
		current = nextBlock;
	}

	chain->head = NULL;
	chain->rear = NULL;
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