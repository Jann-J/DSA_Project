#include<stdio.h>
#include"header.h"

int main(int argc, char *argv[]){
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	Blockchain chain;
	init_blockchain(&chain);

	BlockData *newBlock;
	
	newBlock = ReadFile(argv[1]);
	
	AddBlock(&chain, newBlock);
	
	printBlockchain(chain);
	return 0;
}
