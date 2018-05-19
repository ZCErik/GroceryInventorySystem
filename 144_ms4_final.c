#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINEAR 1
#define FORM 0
#define STOCK 1
#define CHECKOUT 0
#define MAX_ITEM_NO 500
#define MAX_QTY 999
#define SKU_MAX 999
#define SKU_MIN 100
#define DATAFILE "144_fp_items.txt";

// ---------------------------------------
// place function PROTOTYPES below here...
// ---------------------------------------

// user interface tools function PROTOTYPES:

char answer;

/* ms1*/
void welcome(void); //prints a welcome message
void GroceryInventorySystem(void); //print the menu and call all the other functions
int menu(void); // print a menu
void printTitle(void); //print the title
void printFooter(double gTotal); //print the footer with the gTotal
void pause(void); //wait for Enter 
void flushKeyboard(void); //clean the keyboard
int getInt(void); // gets a valid integer
double getDouble(void); // gets a valid double
int getIntLimited(int lowerLimit, int upperLimit); //gets a integer between lower and upper
double getDoubleLimited(double lowerLimit, double upperLimit); //gets a double between lower and upper
int yes(void); //accepts y or n. If y is pressed returns 0 otherwise 1

/* ms2*/
struct Item {
	double price;
	int sku;
	int isTaxed;
	int quantity;
	int minQuantity;
	char name[21];
};

double totalAfterTax(struct Item item);//calcule the total after tax
int isLowQuantity(struct Item item); //check if it has low quantity on the inventory
struct Item itemEntry(int sku); //entry an item on the inventory
void displayItem(struct Item item, int linear); // display item on two ways, linear or form
void listItems(const struct Item item[], int NoOfItems); //list all the items
int locateItem(const struct Item item[], int NoOfRecs, int sku, int* index); // locate an item and its index

//ms3
void search(const struct Item item[], int NoOfRecs);//search for an item
void updateItem(struct Item* itemptr);//update the item in the array
void addItem(struct Item item[], int *NoOfRecs, int sku);//add an item to the inventory
void addOrUpdateItem(struct Item item[], int* NoOfRecs); //add or update an item.
void adjustQuantity(struct Item item[], int NoOfRecs, int stock); //change the quantity

//ms4
void saveItem(struct Item item, FILE* dataFile); //saves the item
int loadItem(struct Item* item, FILE* dataFile); //load the item
int saveItems(const struct Item item[], char fileName[], int NoOfRecs);
int loadItems(struct Item item[], char fileName[], int* NoOfRecsPtr);

const double TAXES = 0.13;
int searchIndex;

// application user interface function PROTOTYPES:

// ----------------------------------------
// place function DEFINITIONS below here...
// 

int main(void){
	GroceryInventorySystem();
	return 0;
}

int menu(void) {
	//shows the menu
	printf("1- List all items\n");
	printf("2- Search by SKU\n");
	printf("3- Checkout an item\n");
	printf("4- Stock an item\n");
	printf("5- Add new item or update item\n");
	printf("6- Delete item\n");
	printf("7- Search by name\n");
	printf("0- Exit program\n");
	printf("> ");

	return 0;
}

void GroceryInventorySystem(void) {
	int option = 0;
	int flag = 1;
	char singleChar;
	struct Item item[MAX_ITEM_NO];
	int recordsRead;
	char file[] = DATAFILE;

	welcome();

	int load = loadItems(item, file, &recordsRead);

	if (load == 0) {
		printf("Could not read from %s\n", file);
	} else {

		while (flag != 0) {

			menu();
			option = getIntLimited(0, 7);

			switch (option) {

			case 0:
				printf("Exit the program? (Y)es/(N)o: ");
				singleChar = yes();

				if ((singleChar == 'Y') || (singleChar == 'y')) {
					//if yes
					flag = 0;
				}
				else
					//if no
					flag = 1;
				break;

			case 1:
				listItems(item, recordsRead);
				pause();
				break;

			case 2:
				search(item, recordsRead);
				pause();
				break;

			case 3:
				adjustQuantity(item, recordsRead, CHECKOUT);
				saveItems(item, file, recordsRead);
				pause();
				break;

			case 4:
				adjustQuantity(item, recordsRead, STOCK);
				saveItems(item, file, recordsRead);
				pause();
				break;

			case 5:
				addOrUpdateItem(item, &recordsRead);

				if(saveItems(item, file, recordsRead) == 0){
					printf("could not update data file %s\n", file);
				}
				pause();
				break;

			case 6:
				printf("Not implemented!\n");
				pause();
				break;

			case 7:
				printf("Not implemented!\n");
				pause();
				break;

			default:
				break;
			}
		}
	}
}


int loadItems(struct Item item[], char fileName[], int* NoOfRecsPtr){
	FILE *fp = NULL;

	fp = fopen(fileName, "r");
	int fileOpen = 0;
	int itemsRead = 0;
	

	if (fp != NULL) {
		fileOpen = 1;	
		while (loadItem(&item[itemsRead], fp) == 0)  {
			itemsRead++;
		}

	}
	*NoOfRecsPtr = itemsRead;
	fclose(fp);
	return fileOpen;	
}

int saveItems(const struct Item item[], char fileName[], int NoOfRecs){
	FILE *fp = NULL;

	fp = fopen(fileName, "w");
	int fileOpen = 0;
	
	if (fp != NULL) {
		fileOpen = 1;
		int i;
		for (i = 0; i < NoOfRecs; i++) {
			saveItem(item[i], fp);
		}	
	}

	fclose(fp);
	return fileOpen;
}

int loadItem(struct Item* item, FILE* dataFile) {
	//sku,quantity,minQuantity,price,isTaxed,name<NEWLINE>
	int trueOrFalse = 0;
	int readAll = fscanf(dataFile, "%d,%d,%d,%lf,%d,%[^\n]", &item->sku, &item->quantity, &item->minQuantity, &item->price, 
															 &item->isTaxed, item->name);
		if (readAll == EOF) {
			trueOrFalse = 1;
		}
	return trueOrFalse;
}
void saveItem(struct Item item, FILE* dataFile) {
	//sku,quantity,minQuantity,price,isTaxed,name<NEWLINE>
	fprintf(dataFile, "%d,%d,%d,%.2lf,%d,%s\n", item.sku, item.quantity, item.minQuantity, item.price, item.isTaxed, item.name);
}


void welcome(void) {
	printf("---=== Grocery Inventory System ===---");
	printf("\n\n");
}

double totalAfterTax(struct Item item) {

	double totalAfterTax = (item.quantity * item.price);

	if (item.isTaxed == 1) {
		totalAfterTax = totalAfterTax + (totalAfterTax * TAXES);
	}

	return totalAfterTax;
}

int isLowQuantity(struct Item item) {

	int isLowQuantity = 0;
	if (item.quantity <= item.minQuantity) {
		isLowQuantity = 1;
	}

	return isLowQuantity;
}

struct Item itemEntry(int sku) {

	struct Item entry;
	entry.sku = sku;

	printf("        SKU: %d\n", entry.sku);
	printf("       Name: ");
	scanf("%20[^\n]", entry.name);
	flushKeyboard();
	printf("      Price: ");
	entry.price = getDoubleLimited(0.01, 1000.00);
	printf("   Quantity: ");
	entry.quantity = getIntLimited(1, MAX_QTY);
	printf("Minimum Qty: ");
	entry.minQuantity = getIntLimited(0, 100);
	printf("   Is Taxed: ");
	entry.isTaxed = yes();

	return entry;
};

void displayItem(struct Item item, int linear) {

	struct item;
	double totalPrice = 0;

	totalPrice = totalPrice + totalAfterTax(item);

	if (linear == 1) {
		printf("|%3d|%-20s|%8.2lf|", item.sku, item.name, item.price);

		if (item.isTaxed == 1) {
			printf("  Yes| %3d | %3d |%12.2lf|", item.quantity, item.minQuantity, totalPrice);
		}
		else
			printf("   No| %3d | %3d |%12.2lf|", item.quantity, item.minQuantity, totalPrice);
		if (item.quantity <= item.minQuantity) {
			printf("***\n");
		}
		else {
			printf("\n");
		}
	}

	if (linear == 0) {
		printf("        SKU: %d\n", item.sku);
		printf("       Name: %s\n", item.name);
		printf("      Price: %.2lf\n", item.price);
		printf("   Quantity: %d\n", item.quantity);
		printf("Minimum Qty: %d\n", item.minQuantity);

		if (item.isTaxed == 1) {
			printf("   Is Taxed: Yes\n");
		}
		else {
			printf("   Is Taxed: No\n");
		}
		if (item.quantity <= item.minQuantity) {
			printf("WARNING: Quantity low, please order ASAP!!!\n");
		}
	}
}

void listItems(const struct Item item[], int NoOfItems) {
	//receives an array of items and their numbers on it and prints the items list with the grand total price at the end. 
	int i;
	double grandTotal = 0;
	printTitle();

	for (i = 0; i < NoOfItems; ++i) {
		printf("%-4d", i + 1);
		displayItem(item[i], LINEAR);
		grandTotal = grandTotal + totalAfterTax(item[i]);
	}

	printFooter(grandTotal);
}

int locateItem(const struct Item item[], int NoOfRecs, int sku, int* index) {
	int i;
	int trueValue = 0;
	for (i = 0; i < NoOfRecs; ++i) {
		if (item[i].sku == sku) {
			*index = i;
			trueValue = 1;
			break;
		}
	}
	return trueValue;
}

void flushKeyboard(void) {
	while (getchar() != '\n');
}

void pause(void) {
	printf("Press <ENTER> to continue...");
	flushKeyboard();
}

int yes(void) {
	// accepts yes or no only, and the return the value
	char singleChar;
	int flag = 0;
	int validate = 0;

	do {
		scanf(" %c", &singleChar);
		flushKeyboard();

		if ((singleChar != 'Y') && (singleChar != 'y') && (singleChar != 'N') && (singleChar != 'n')) {
			printf("Only (Y)es or (N)o are acceptable: ");
		}
		else {
			flag = 1;
		}

	} while (flag != 1);

	if ((singleChar == 'Y') || (singleChar == 'y')) {
		validate = 1;
	}

	return validate;
}

double getDouble() {
	double value;
	char nl;

	do {
		scanf("%lf%c", &value, &nl);

		if (nl != '\n') {
			flushKeyboard();
			printf("Invalid number, please try again: ");
		}

	} while (nl != '\n');

	return value;
}

double getDoubleLimited(double lowerLimit, double upperLimit) {
	double value;

	value = getDouble();

	while ((value < lowerLimit) || (value > upperLimit)) {
		printf("Invalid value, %lf< value < %lf: ", lowerLimit, upperLimit);
		value = getDouble();
	}

	return value;
}

int getInt(void) {
	int  value;
	char nl;

	do {
		scanf("%d%c", &value, &nl);

		if (nl != '\n') {
			flushKeyboard();
			printf("Invalid integer, please try again: ");
		}

	} while (nl != '\n');

	return value;
}

int getIntLimited(int lowerLimit, int upperLimit) {
	int value;

	value = getInt();

	while ((value < lowerLimit) || (value > upperLimit)) {
		printf("Invalid value, %d < value < %d: ", lowerLimit, upperLimit);
		value = getInt();
	}

	return value;
}

void printTitle(void) {
	printf("Row |SKU| Name               | Price  |Taxed| Qty | Min |   Total    |Atn\n");
	printf("----+---+--------------------+--------+-----+-----+-----+------------|---\n");
}

void printFooter(double gTotal) {

	printf("--------------------------------------------------------+----------------\n");
	if (gTotal > 0) {
		/* code */
		printf("                                           Grand Total: |%12.2lf\n", gTotal);
	}
}

void updateItem(struct Item* itemptr) {
	char answer;
	struct Item newData;

	printf("Enter new data:\n");
	newData = itemEntry(itemptr->sku);

	printf("Overwrite old data? (Y)es/(N)o: ");
	answer = yes();
	if (answer) {
		itemptr->price = newData.price;
		itemptr->isTaxed = newData.isTaxed;
		itemptr->quantity = newData.quantity;
		itemptr->minQuantity = newData.minQuantity;
		strcpy(itemptr->name, newData.name);
		printf("--== Updated! ==--\n");

	}
	else {
		printf("--== Aborted! ==--\n");
	}
}

void search(const struct Item item[], int NoOfRecs) {

	printf("Please enter the SKU: ");
	int searchFor = getIntLimited(SKU_MIN, SKU_MAX);
	int index;
	int exist = locateItem(item, NoOfRecs, searchFor, &index);

	if (exist) {
		displayItem(item[index], FORM);
	}
	else {
		printf("Item not found!\n");
	}
}

void addItem(struct Item item[], int *NoOfRecs, int sku) {
	struct Item newItem;
	char answer;

	if (*NoOfRecs == MAX_ITEM_NO) {
		printf("Can not add new item; Storage Full!\n");
	}
	else {
		newItem = itemEntry(sku);
		printf("Add Item? (Y)es/(N)o: ");
		answer = yes();

		if (answer) {
			item[*NoOfRecs].sku = newItem.sku;
			item[*NoOfRecs].price = newItem.price;
			item[*NoOfRecs].isTaxed = newItem.isTaxed;
			item[*NoOfRecs].quantity = newItem.quantity;
			item[*NoOfRecs].minQuantity = newItem.minQuantity;
			strcpy(item[*NoOfRecs].name, newItem.name);
			(*NoOfRecs)++;
			printf("--== Added! ==--\n");
		}
		else {
			printf("--== Aborted! ==--\n");
		}
	}
}

void addOrUpdateItem(struct Item item[], int* NoOfRecs) {
	int index;
	char answer;

	printf("Please enter the SKU: ");
	int searchFor = getIntLimited(SKU_MIN, SKU_MAX);
	int exist = locateItem(item, *NoOfRecs, searchFor, &index);

	if (exist) {
		displayItem(item[index], FORM);
		printf("Item already exists, Update? (Y)es/(N)o: ");
		answer = yes();
		if (answer) {
			updateItem(&item[index]);
		}
		else {
			printf("--== Aborted! ==--\n");
		}
	}
	else {
		addItem(item, NoOfRecs, searchFor);
	}
}

void adjustQuantity(struct Item item[], int NoOfRecs, int stock) {
	int index;
	char increase[] = "to stock";
	char decrease[] = "to checkout";

	printf("Please enter the SKU: ");
	int searchFor = getIntLimited(SKU_MIN, SKU_MAX);
	int exist = locateItem(item, NoOfRecs, searchFor, &index);

	int qtyAdjust;

	if (exist) {
		displayItem(item[index], FORM);
		if (stock > 0) {
			int maxQty = (MAX_QTY - item[index].quantity);
			printf("Please enter the quantity %s; Maximum of %d or 0 to abort: ", increase, maxQty);
			qtyAdjust = getIntLimited(0, maxQty);
			if (qtyAdjust == 0) {
				printf("--== Aborted! ==--\n");
			}
			else {
				item[index].quantity = item[index].quantity + qtyAdjust;
				printf("--== Stocked! ==--\n");
			}
		}
		else {
			printf("Please enter the quantity %s; Maximum of %d or 0 to abort: ", decrease, item[index].quantity);
			qtyAdjust = getIntLimited(0, item[index].quantity);
			if (qtyAdjust == 0) {
				printf("--== Aborted! ==--\n");
			}
			else {
				item[index].quantity = item[index].quantity - qtyAdjust;
				printf("--== Checked out! ==--\n");

				if (isLowQuantity(item[index])) {
					printf("Quantity is low, please reorder ASAP!!!\n");
				}
			}
		}
	}
	else {
		printf("SKU not found in storage!\n");
	}
}
