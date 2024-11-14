/* strtok() is destructive, meaning it changes the 
* input string by inserting null characters (\0).
* If you need the original row after tokenization,
* create a copy of it before calling strtok(). */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"header.h"

/* Initialises Blockchain */
void init_blockchain(Blockchain *chain){
	chain->head = NULL;
	chain->rear = NULL;
	return;
}


/* takes parameters string filename and
* read first line of header
* stored second line for index, time, nonce, sender, reciever, itemcount in struct
* stored item information like itemname, amount, quantity in struct
* returns the struct BlockData read from file */
BlockData *ReadFile(char *filename){
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		perror("Error: Unable to open file. Please check if the file exists.\n\n");
		return NULL;
	}
	
	char row[MAX_LINE], *token, ch;
	int i, itemIndex = 0;
	
	//Error in block data
	BlockData *blockData = (BlockData *)malloc(sizeof(BlockData));
	if (blockData == NULL){
		fprintf(stderr, "Error: Memory allocation failed for blockData.\n");
		fclose(fp);
	return NULL;
	}
	
	//here title row is just read
	while((ch = fgetc(fp)) != '\n');
	//insufficient data in title row
	if (ch == EOF) {
		fprintf(stderr, "Error: File does not contain sufficient data.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	
	//reading initial info of block
	i = 0;
	while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
		if (i < MAX_LINE - 1){
			row[i] = ch;
			i++;
		}
	}
	//insufficient data in second row
	if (ch == EOF && i == 0) {
		fprintf(stderr, "Error: File does not contain sufficient data for block information.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	row[i] = '\0';
	
	//Parsing block data
	token = strtok(row, ",");
	if (token == NULL) {
		fprintf(stderr, "Error: Insufficient block data in the file.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	blockData->index = strtoul(token, NULL, 10);  // Block index
		
	token = strtok(NULL, ",");
	if (token == NULL) {
		fprintf(stderr, "Error: Insufficient block data(missing sender) in the file.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	strncpy(blockData->info.sender, token, NAME_SIZE - 1); // Sender
	blockData->info.sender[NAME_SIZE - 1] = '\0';
		
	token = strtok(NULL, ",");
	if (token == NULL) {
		fprintf(stderr, "Error: Insufficient block data(missing receiver) in the file.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	strncpy(blockData->info.receiver, token, NAME_SIZE - 1); // Receiver
	blockData->info.receiver[NAME_SIZE - 1] = '\0';
		
	token = strtok(NULL, ",");
	if (token == NULL) {
		fprintf(stderr, "Error: Insufficient block data(missing item number in the file.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	blockData->info.itemCount = strtoul(token, NULL, 10);  // Number of items
	if (blockData->info.itemCount == 0) {
		fprintf(stderr, "Error: Item count must be greater than zero.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	
	// Allocate memory for items
        blockData->info.items = (item *)malloc(blockData->info.itemCount * sizeof(item));
	if (blockData->info.items == NULL) {
		fprintf(stderr, "Memory allocation failed for items.\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	
	// Read item details
        while (itemIndex < blockData->info.itemCount && fgets(row, MAX_LINE, fp) != NULL) {
		token = strtok(row, ",");
		if (token == NULL) {
			fprintf(stderr, "Error: Insufficient item data (missing item name) at Index: %d\n", itemIndex);
			free(blockData->info.items);
			free(blockData);
			fclose(fp);
			return NULL;
		}
		strncpy(blockData->info.items[itemIndex].itemName, token, NAME_SIZE - 1);
		blockData->info.items[itemIndex].itemName[NAME_SIZE - 1] = '\0'; // itemName
		
		token = strtok(NULL, ",");
		if (token == NULL) {
			fprintf(stderr, "Error: Insufficient item data (missing item name) at Index: %d\n", itemIndex);
			free(blockData->info.items);
			free(blockData);
			fclose(fp);
			return NULL;
		}
                blockData->info.items[itemIndex].quantity = strtoul(token, NULL, 10); //itemQuantity
                
		token = strtok(NULL, ",");
		if (token == NULL) {
			fprintf(stderr, "Error: Insufficient item data (missing item name) at Index: %d\n", itemIndex);
			free(blockData->info.items);
			free(blockData);
			fclose(fp);
			return NULL;
		}
                blockData->info.items[itemIndex].amount = strtof(token, NULL); //itemAmount
                itemIndex++;
        }
        
        // Error in item count
        if(itemIndex < blockData->info.itemCount){
		fprintf(stderr, "Error: File does not contain enough item details.\n");
		free(blockData->info.items);
		free(blockData);
		fclose(fp);
		return NULL;
	}
        
	fclose(fp);
	return blockData;
}

/* Adds calculated data to BlockData like
* timestamp, prevHash, currHash, merkleRoot
* and nonce */
void addInfoToBlock(BlockData *blockData){
	// Set the timestamp to the current time
	blockData->timestamp = time(NULL);

	// For testing, use dummy data for hashes
	memcpy(blockData->prevHash, "prevHash", 8);
	memcpy(blockData->currHash, "CurrHash", 8);
	memcpy(blockData->merkleRoot, "merkleRoot", 10);
	blockData->nonce = 1234;
	return;
}

/* Adds block in blockchain
* later add check for no repeated block*/
void AddBlock(Blockchain *chain, BlockData *blockData){
	block *newBlock = (block *)malloc(sizeof(block));
	if(newBlock == NULL){
		perror("Error: Memory allocation failed.\n");
		return;
	}
	
	newBlock->data = *blockData;
	newBlock->next = NULL;
	addInfoToBlock(blockData);
	// Genesis block
	if(chain->head == NULL){
		chain->head = newBlock;
		chain->rear = newBlock;
		printf("Block added successfully.\n");
		return;
	}
	printf("hey3\n");
	chain->rear->next = newBlock;
	chain->rear = newBlock;
	printf("Block added successfully.\n");
	return;
}

void printTimestamp(time_t timestamp) {
	struct tm *tm_info = localtime(&timestamp);
	char buffer[26];
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	printf("%s", buffer);
	return;
}

void printHash(unsigned char *hash, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
}

/* Prints block based off index*/
void printBlock(Blockchain chain, int index){
		block *current = chain.head;
		while(current != NULL && index != current->data.index)
			current = current->next;
			
		if (current == NULL) {
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

		// Print sender and receiver
		printf("Sender: %s\n", data.info.sender);
		printf("Receiver: %s\n", data.info.receiver);

		// Print item details
		printf("Items:\n");
		for (int i = 0; i < data.info.itemCount; i++) {
			printf("\tItem Name [%d]: %s\n", i, data.info.items[i].itemName);
			printf("\tQuantity [%d]: %u\n", i, data.info.items[i].quantity);
			printf("\tAmount [%d]: %.2f\n", i, data.info.items[i].amount);
			putchar('\n');
		}
		
		// Print hash details
		printf("Previous Hash: ");
		printHash(data.prevHash, SHA256_DIGEST_LENGTH);
		printf("\n");

		printf("Current Hash: ");
		printHash(data.currHash, SHA256_DIGEST_LENGTH);
		printf("\n");

		printf("Merkle Root: ");
		printHash(data.merkleRoot, SHA256_DIGEST_LENGTH);
		printf("\n");
		
		// Print nonce
		printf("Nonce: %u\n", data.nonce);
		return;
	
}

/* Prints whole Blockchain*/
void printBlockchain(Blockchain chain){
	block *current = chain.head; // Start from the head of the blockchain

	printf("=========== Blockchain ===========\n\n");
	while (current != NULL) {
		BlockData data = current->data;
		
		// Print Index
		printf("Block Index: %u\n", data.index);
		
		// Print time
		printf("Timestamp: ");
		printTimestamp(data.timestamp);
		printf("\n");

		// Print sender and receiver
		printf("Sender: %s\n", data.info.sender);
		printf("Receiver: %s\n", data.info.receiver);

		// Print item details
		for (int i = 0; i < data.info.itemCount; i++) {
			printf("\tItem Name [%d]: %s\n", i, data.info.items[i].itemName);
			printf("\tQuantity [%d]: %u\n", i, data.info.items[i].quantity);
			printf("\tAmount [%d]: %.2f\n", i, data.info.items[i].amount);
			putchar('\n');
		}
		
		// Print hash details
		printf("Previous Hash: ");
		printHash(data.prevHash, SHA256_DIGEST_LENGTH);
		printf("\n");

		printf("Current Hash: ");
		printHash(data.currHash, SHA256_DIGEST_LENGTH);
		printf("\n");

		printf("Merkle Root: ");
		printHash(data.merkleRoot, SHA256_DIGEST_LENGTH);
		printf("\n");
		
		// Print nonce
		printf("Nonce: %u\n", data.nonce);
		printf("----------------------------------\n\n");

		current = current->next;
	}
	return;
}

/* Frees Blockchain after program is completed*/
void freeBlockchain(Blockchain *chain) {
	block *current = chain->head;
	while (current != NULL) {
		block *nextBlock = current->next;

		// Free items in BlockData
		if (current->data.info.items != NULL)
			free(current->data.info.items);
        
		// Free the block itself
		free(current);
		current = nextBlock;
	}
    
	chain->head = NULL;
	chain->rear = NULL;
	return;
}
