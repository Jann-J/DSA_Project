#include <stdio.h>
#include "header.h"
#define FILE_NAME_SIZE 16

void displayMenu()
{
	printf("\n==================== Blockchain Supply Chain Management ====================\n\n");
	printf("1. Add a New Block from CSV File\n");
	printf("2. View Blockchain\n");
	printf("3. Edit a Block\n");
	printf("4. Validate Blockchain Integrity\n");
	printf("5. Search Transactions\n");
	printf("   a. By Sender\n");
	printf("   b. By Receiver\n");
	printf("   c. By Item Name\n");
	printf("6. Display Block Details\n");
	printf("7. Export Blockchain Data to CSV\n");
	printf("8. Import Blockchain Data from CSV\n");
	printf("9. Network Options\n");
	printf("   a. Share Block via P2P Network\n");
	printf("   b. Receive Block from P2P Network\n");
	printf("10. Exit\n");
	printf("11. Make a transaction\n");
	printf("12. Your Profile\n");
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

// for first transaction to avoid conflict
int FIRST_TRANSACTION = 0;

int main()
{
	// Login To Your Account
	CreateAccount();

	// Give Random Wallete Balance to Nodes
	if (FIRST_TRANSACTION == 0)
	{
		CreatesNodesWithRandomBalance();
		FIRST_TRANSACTION = 1;
	}

	int choice, index;
	char subChoice;
	char filename[FILE_NAME_SIZE];

	Blockchain chain;
	init_blockchain(&chain);
	while (1)
	{
		displayMenu();
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			printf("Option 1: Add a New Block from CSV File selected.\n");
			printf("Enter .csv filename for adding the block: ");
			scanf("%s", filename);
			if (ReadFile(filename) == NULL)
				break;
			AddBlock(&chain, (ReadFile(filename)));
			break;
		case 2:
			printf("Option 2: View Blockchain selected.\n");
			printBlockchain(chain);
			break;
		case 3:
			printf("Option 3: Edit a Block selected.\n");
			break;
		case 4:
			printf("Option 4: Validate Blockchain Integrity selected.\n");
			isBlockChainValid(chain);
			break;
		case 5:
			printf("Option 5: Search Transactions selected.\n");
			printf("   Enter sub-choice (a/b/c): ");
			scanf(" %c", &subChoice); // Note the space before %c to handle newline characters
			switch (subChoice)
			{
			case 'a':
				printf("   Sub-option a: By Sender selected.\n");
				break;
			case 'b':
				printf("   Sub-option b: By Receiver selected.\n");
				break;
			case 'c':
				printf("   Sub-option c: By Item Name selected.\n");
				break;
			default:
				printf("   Invalid sub-choice. Please try again.\n");
				break;
			}
			break;
		case 6:
			printf("Option 6: Display Block Details selected.\n");
			printf("Enter Index of block to be displayed: ");
			scanf("%d", &index);
			printBlock(chain, index);
			break;
		case 7:
			printf("Option 7: Export Blockchain Data to CSV selected.\n");
			break;
		case 8:
			printf("Option 8: Import Blockchain Data from CSV selected.\n");
			break;
		case 9:
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
		case 10:
			printf("Exiting the program. Goodbye!\n");
			freeBlockchain(&chain);
			return 0;
		case 11:
			txInfo *newtx = InputTransactionData();
			// verify transaction & update WalletBank
			ValidateTransactionData(newtx) ? printf("Valid Transaction\n") : printf("Invalid transaction\n");
			break;
		case 12:
			ProfileMenu();
			CreateProfileDashboard();
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
	return 0;
}
