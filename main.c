#include<stdio.h>
#include"header.h"

void displayMenu() {
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
	printf("\n===========================================================================\n");
}

int main(){
	int choice;
	char subChoice;
	
	Blockchain chain;
	while(1){
		displayMenu();
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		switch(choice){
			case 1:
				printf("Option 1: Add a New Block from CSV File selected.\n");
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
				break;
			case 5:
				printf("Option 5: Search Transactions selected.\n");
		        	printf("   Enter sub-choice (a/b/c): ");
		        	scanf(" %c", &subChoice); // Note the space before %c to handle newline characters
				switch (subChoice) {
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
				switch(subChoice){
					case 'a':
						printf("   Sub-option a: Share Block via P2P Network selected.\n");
						break;
					case 'b':
						printf("   Sub-option b: Receive Block from P2P Network selected.\n");
						break;
					default:
						printf("   Invalid sub-choice. Please try again.\n");
						break;
				}
				break;
			case 10:
				printf("Exiting the program. Goodbye!\n");
				return 0;
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
