/*
* strtok() is destructive, meaning it changes the 
* input string by inserting null characters (\0).
* If you need the original row after tokenization,
* create a copy of it before calling strtok().
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define NAME_SIZE 50
#define MAX_LINE 128
#define SHA256_DIGEST_LENGTH 32

#include"header.h"

//Initialises Blockchain
void init_blockchain(Blockchain *chain){
	chain->head = NULL;
	chain->rear = NULL;
	return;
}

/*
* takes parameters string filename and
*read first line of header
*stored second line for index, time, nonce, sender, reciever, itemcount in struct
*stored item information like itemname, amount, quantity in struct
*returns the struct BlockData read from file
*/
BlockData *ReadFile(char *filename){
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		perror("Error opening file.\n");
		return NULL;
	}
	
	char row[MAX_LINE], *token, ch;
	int i, itemIndex = 0;
	
	BlockData *blockData = (BlockData *)malloc(sizeof(BlockData));
	if (blockData == NULL){
		fprintf(stderr, "Memory allocation error for blockData!\n");
		fclose(fp);
	return NULL;
	}
	
	//here title row is just read
	while((ch = fgetc(fp)) != '\n');
	if (ch == EOF) return NULL;
	
	//reading initial info of block
	i = 0;
	while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
		if (i < MAX_LINE - 1){
			row[i] = ch;
			i++;
		}
	}
	if (ch == EOF && i == 0) return NULL;
	row[i] = '\0';
	
	token = strtok(row, ",");
	while (token != NULL) {
		if (token != NULL) {
			blockData->index = strtoul(token, NULL, 10);  // Block index
			token = strtok(NULL, ",");
			printf("%u\n", blockData->index);
		}
		if (token != NULL) {
			blockData->nonce = strtoul(token, NULL, 10);  // Nonce
			token = strtok(NULL, ",");
			printf("%u\n", blockData->nonce);
		}
		if (token != NULL) {
			strncpy(blockData->info.sender, token, NAME_SIZE - 1); // Sender
			blockData->info.sender[NAME_SIZE - 1] = '\0';
			token = strtok(NULL, ",");
			printf("%s\n", blockData->info.sender);
		}
		if (token != NULL) {
			strncpy(blockData->info.receiver, token, NAME_SIZE - 1); // Receiver
			blockData->info.receiver[NAME_SIZE - 1] = '\0';
			token = strtok(NULL, ",");
			printf("%s\n", blockData->info.receiver);
		}
		if (token != NULL) {
			blockData->info.itemCount = strtoul(token, NULL, 10);  // Number of items
			printf("%zu\n", blockData->info.itemCount);
			token = strtok(NULL, ",");
		}
		
	}
	
	 // Allocate memory for items
        blockData->info.items = (item *)malloc(blockData->info.itemCount * sizeof(item));
	if (blockData->info.items == NULL) {
		fprintf(stderr, "Memory allocation error for items!\n");
		free(blockData);
		fclose(fp);
		return NULL;
	}
	
	// Read item details
        while (itemIndex < blockData->info.itemCount && fgets(row, MAX_LINE, fp) != NULL) {
            token = strtok(row, ",");
            if (token != NULL) {
                strncpy(blockData->info.items[itemIndex].itemName, token, NAME_SIZE - 1);
                blockData->info.items[itemIndex].itemName[NAME_SIZE - 1] = '\0';
                token = strtok(NULL, ",");
            }
            if (token != NULL) {
                blockData->info.items[itemIndex].quantity = strtoul(token, NULL, 10);
                token = strtok(NULL, ",");
            }
            if (token != NULL) {
                blockData->info.items[itemIndex].amount = strtof(token, NULL);
            }
            itemIndex++;
        }
        
	fclose(fp);
	return blockData;
}

void AddBlock(Blockchain *chain, BlockData *blockData){
    
}

/*function reads from file given
*stores it in data structure and prev hash
*it then calles for hash function and merkleroot
* fills it in the block
*/

//void newBlock(Blockchain *b, )

//print block

//print blockchain

/* test cases
* if file info is incomplete print an error
* 
*/
