#include "SymbolTable.h"


Table* makeTable(Table* currTable) {
	// I create a Table that contains symbolArray which is size of 26(the amount of english letters). Each cell in the table is a linked list represent the same first letter of char_id(the key of the hashtable).
	// The first element of every cell is a dummy entry.
	Table* table = (Table*)malloc(sizeof(Table));
	table->father = currTable;

	for (int i = 0; i < SIZE; i++) {
		TableEntry* dummyEntry = (TableEntry*)malloc(sizeof(TableEntry));
		dummyEntry->prev = NULL;
		dummyEntry->next = NULL;
		table->symbolsArray[i] = dummyEntry;
	}
	return table;
}

Table* popTable(Table* currTable) {

	freeTableMemory(currTable);
	return currTable->father;
}

TableEntry* insert(Table* currTable, char* name) {

	TableEntry* entry = lookUp(currTable, name);
	if (entry != NULL) {

		return NULL;
	}

	return(createNewEntry(currTable, name));
}

TableEntry* createNewEntry(Table* currTable, char* name) {

	int index = hashCode(name);
	TableEntry* dummyEntry = currTable->symbolsArray[index];
	// Find the next entry in the linked list of the hash table in same index
	while (dummyEntry->next != NULL) dummyEntry = dummyEntry->next;
	// Create the new entry
	dummyEntry->next = (TableEntry*)malloc(sizeof(TableEntry));

	TableEntry* newEntry = dummyEntry->next;
	// Set the proper name and role of the same entry
	setEntryName(newEntry, name);
	// Defualt values for new entry
	setUsedEntry(FALSE, newEntry);
	setActualReturnType(newEntry, TOKEN_VOID);
	newEntry->prev = dummyEntry;
	newEntry->type = TOKEN_VOID;
	newEntry->next = NULL;
	newEntry->dimensions = NULL;
	newEntry->funcSignature = NULL;
	newEntry->hasReturn = FALSE;
	return newEntry;
}
// Perform local search for name
TableEntry* lookUp(Table* currTable, char* name) {

	int index = hashCode(name);
	TableEntry* entry = currTable->symbolsArray[index];
	int found = FALSE;
	// First entry is a dummy entry
	entry = entry->next;

	while (entry != NULL && !found) {

		if (strcmp(entry->entryName, name) == 0) found = TRUE;

		else entry = entry->next;
	}

	// If not found the proper entry on the while loop, entry will hold NULL value(this is the coniditon stop for the while loop)
	// Else entry holds the proper entry pointer
	return entry;
}
// Perform hierarchical search
TableEntry* find(Table* currTable, char* name) {

	Table* searchTable = currTable;
	TableEntry* entry = NULL;
	int found = 0;

	while (searchTable != NULL && !found) {
		// Look for the name locally
		entry = lookUp(searchTable, name);

		if (entry != NULL) found = 1;

		searchTable = searchTable->father;
	}

	return entry;
}

/* Setters Getters*/
void setType(TableEntry* idEntry, TOKEN_TYPE type) {
	idEntry->type = type;
}

TOKEN_TYPE getType(TableEntry* idEntry) {
	return idEntry->type;
}

void setDimensions(TableEntry* idEntry, dimensionElem* dimensionSizes) { idEntry->dimensions = dimensionSizes; }

dimensionElem* getDimensions(TableEntry* idEntry) { return idEntry->dimensions; }

Role getRole(TableEntry* idEntry) { return idEntry->role; }

void freeTableMemory(Table* table) {

	TableEntry** entryArray = table->symbolsArray;
	// First free all memory of the entries of the symbol array
	for (int i = 0; i < SIZE; i++) {

		// The first element in each cell is a dummy element and needs to be deallocated diffrently
		TableEntry* dummEntry = entryArray[i];
		TableEntry* entry = dummEntry->next;
		while (entry != NULL) {

			TableEntry* temp = entry->next;
			freeEntry(entry);

			entry = temp;
		}
		free(dummEntry);
	}
}

void freeEntry(TableEntry* entry) {

	if (entry->funcSignature != NULL)
		free(entry->funcSignature);
	if (entry->dimensions != NULL)
		free(entry->dimensions);
	free(entry);
}

int hashCode(char* name) {
	// The hash function is the first letter of the name module the size of the symbol array
	char ch = tolower(name[0]);
	return ch - 'a';
}

void setRole(TableEntry* idEntry, Role role) { idEntry->role = role; }

void setEntryName(TableEntry* currEntry, char* name) { currEntry->entryName = name; }

char* getEntryName(TableEntry* currEntry) { return currEntry->entryName; }

void addDimension(TableEntry* currEntry, int dimension) {

	dimensionElem* head = currEntry->dimensions;

	int size = 1;
	if (head == NULL) {
		head = malloc(sizeof(dimensionElem));
		head->dimension = dimension;
		head->next = NULL;
		setDimensions(currEntry, head);
	}
	else {
		dimensionElem* temp = head;
		size++;
		while (temp->next != NULL) {
			temp = temp->next;
			size++;
		}
		temp->next = (dimensionElem*)malloc(sizeof(dimensionElem));
		temp = temp->next;
		temp->next = NULL;
		temp->dimension = dimension;
	}
	head->size = size;
}

TableEntry* getNewDummyEntry() {
	// This functions creates and return a new empty TableEntry which is linked to nothing
	TableEntry* entryPtr = (TableEntry*)malloc(sizeof(TableEntry));
	entryPtr->next = NULL;
	entryPtr->prev = NULL;
	entryPtr->dimensions = NULL;
	entryPtr->funcSignature = NULL;
	entryPtr->entryName = "";
	entryPtr->type = TOKEN_VOID;
	entryPtr->actualReturnType = TOKEN_VOID;
	entryPtr->role = NONE;
	entryPtr->hasReturn = FALSE;

	return entryPtr;
}

void copyEntryValuesToRealEntry(TableEntry* dummyEntry, TableEntry* realEntry) {
	// This functions copies the values of the dummy entry into the readl entry(which is already placed inside the correct table)
	// Not coping the name because realEntry should hold the name(from the createEntry function).
	// Not coping next/prev because createEntry should take care of placing the correct next/prev
	setDimensions(realEntry, getDimensions(dummyEntry));
	setFuncSignature(realEntry, getFuncSignature(dummyEntry));
	setType(realEntry, getType(dummyEntry));
	setRole(realEntry, getRole(dummyEntry));
	setActualReturnType(realEntry, getActualReturnType(dummyEntry));
}

TableEntry* getFuncSignature(TableEntry* entry) { return entry->funcSignature; }

void setFuncSignature(TableEntry* entryPtr, TableEntry* funcSignature) { entryPtr->funcSignature = funcSignature; }

void setUsedEntry(int useState, TableEntry* currEntry) { currEntry->usedEntry = useState; }

int getUsedEntry(TableEntry* currEntry) { return currEntry->usedEntry; }

void setTypeValue(ValueType* item, TOKEN_TYPE type, int value) {

	item->type = type;
	item->value = value;
}

void setActualReturnType(TableEntry* entry, TOKEN_TYPE type) { entry->actualReturnType = type; }

TOKEN_TYPE getActualReturnType(TableEntry* entry) { return entry->type; }

const char* roleEnumToString(Role role) {


	switch (role) {
	case PRE_DEF_FUNC:
		return "Pre function definition";

	case FULL_DEF_FUNC:
		return "Full function definition";

	case VARIABLE:
		return  "Variable";

	case ARRAY:
		return "Arrray";

	default:
		return "Not available role";
	}
}

ValueType* getDummyValueTypeItem() {

	ValueType* item = (ValueType*)malloc(sizeof(ValueType));
	item->next = NULL;
	return item;
}

ValueType* addItemToValueTypeList(ValueType* list, ValueType* item) {

	int size = 1;
	if (list == NULL) {

		list = item;
		list->next = NULL;
	}
	else {
		ValueType* temp = list;
		size++;
		//printf("before loop: %d ", size);
		while (temp->next != NULL) {

			temp = temp->next;
			size++;
			//printf(" in loop: %d ", size);
		}
		//printf("%d\n", size);
		temp->next = item;
	}
	list->size = size;

	return list;
}

void freeValueTypeList(ValueType* item) {

	ValueType* temp = NULL;
	while (item != NULL) {
		temp = item->next;
		free(item);
		item = temp;
	}
}
TableEntry* addItemToList(TableEntry* list, TableEntry* item) {

	if (list == NULL)
		list = item;
	else {
		TableEntry* temp = list;
		while (temp->next != NULL) temp = temp->next;
		temp->next = item;
	}

	return list;
}

int getListSize(TableEntry* list) {

	int size = 0;
	TableEntry* temp = list;
	while (temp != NULL) {
		temp = temp->next;
		size++;
	}

	return size;
}
int getValueTypeListSize(ValueType* list) {

	int size = 0;
	ValueType* temp = list;
	while (temp != NULL) {
		temp = temp->next;
		size++;
	}

	return size;
}

void setHasReturn(TableEntry* currEntry, int returnState) { currEntry->hasReturn = returnState; }

int getHasReturn(TableEntry* currEntry) { return currEntry->hasReturn; }
