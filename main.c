#include <stdio.h>
#include "header.h"
#define FILE_NAME_SIZE 16

void displayMenu()
{
	printf("\n==================== Blockchain Supply Chain Management ====================\n\n");
	printf("1. Your Profile\n");
	printf("2. Add a New Block from CSV File\n");
	printf("3. Validate Blockchain Integrity\n");
	printf("4. Search Transactions\n");
	printf("5. Display Block Details\n");
	printf("6. Network Options\n");
	printf("   a. Share Block via P2P Network\n");
	printf("   b. Receive Block from P2P Network\n");
	printf("7. Exit\n");
	printf("8. View Blockchain.\n");
	printf("\n===========================================================================\n");
}

void ProfileMenu()
{
	printf("\n==============================\n");
	printf("     PROFILE DASHBOARD\n");
	printf("==============================\n");
	printf("Welcome \n");
	printf("------------------------------\n");
	printf("Please choose an option:\n");
	printf("1. Lookup Transaction\n");
	printf("2. View Account Details\n");
	printf("3. Exit\n");
	printf("==============================\n");
}

int main()
{
	/*while(1){
		if(isAuthenticated())
			break;
	}*/	

	int choice, index, i;
	char subChoice;
	char filename[FILE_NAME_SIZE];
	char userID[PUBLIC_ID_SIZE];

	Blockchain chain;
	init_blockchain(&chain);
	HashTable Stable;
	initSHashTable(&Stable);
	InitWalletStorage();

	while (1)
	{
		displayMenu();
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			ProfileMenu();
			CreateProfileDashboard(Stable);
			break;
		case 2:
			printf("Option 1: Add a New Block from CSV File selected.\n");
			printf("Enter .csv filename for adding the block: ");
			scanf("%s", filename);

			BlockData *data = ReadFile(filename);
			if (data == NULL)
				break;

			mergeSort(data->info, 0, data->NumOfTxn - 1);	
					
			if(!isTxnBlockValid(data->info, data->NumOfTxn))
			{
				printf("\nInvalid Transactions Found!!\n");
				printf("Block is NOT added\n");
				free(data);
				break;
			};
			/*
				WalletPrint(); 
				use it to print WalletInfo 
			*/
			printf("\nValid Transactions...\n");

			updateBalance(data->info, data->NumOfTxn);
			AddBlock(&chain, data);

			i = findFirstIndex(data->info, data->NumOfTxn, "public-id-0001");
			if(i == -1) ;
			else 
				insertInSHashTable(i, data->NumOfTxn, data->info, &Stable);

			printf("Block is Added Successfully!!\n");
			
			// Clean Up
			free(data);
			break;
			
		case 3:
			printf("Option 4: Validate Blockchain Integrity selected.\n");
			isBlockChainValid(chain);
			break;
		case 4:
			//printf("Enter private ID: ");
			//scanf("%s", userID);
			//cmp with private id
			printf("Enter Transaction ID to be searched: ");
			scanf("%s", userID);
			Info *info;
			info = searchSHashTable(&Stable, userID);
			if(info){
				printTransactionInfo(*info);
			} else {
				printf("Transaction ID not found.\n");
			}
			break;
		case 5:
			printf("Option 6: Display Block Details selected.\n");
			printf("Enter Index of block to be displayed: ");
			scanf("%d", &index);
			printBlock(chain, index);
			break;
		case 6:
			printf("Option 9: Network Options selected.\n");
			printf("   Enter sub-choice (a/b): ");
			scanf(" %c", &subChoice); // Note the space before %c to handle newline characters
			switch (subChoice)
			{
			case 'a':
				printf("   Sub-option a: Share Block via P2P Network selected.\n");
				char blockname[FILE_NAME_SIZE];
				printf("Enter block name: ");
				scanf("%s", blockname);
				P2P_NetworkConnection(blockname);
				break;
			case 'b':
				printf("   Sub-option b: Receive Block from P2P Network selected.\n");
				P2P_NetworkConnection("block.csv");
				break;
			default:
				printf("   Invalid sub-choice. Please try again.\n");
				break;
			}
			break;
		case 7:
			exit(0);
			break;
		case 8:
			printf("Option 2: View Blockchain selected.\n");
			printBlockchain(chain);
			break;
		default:
			printf("Invalid choice. Please enter a number between 1 and 10.\n");
			break;
		}

		printf("\nPress any key to continue...");
		getchar();
		getchar();
	}
	freeBlockchain(&chain);
	freeSHashTable(&Stable);
	return 0;
}
