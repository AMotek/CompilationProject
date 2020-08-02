#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Token.h"
#include <float.h>

#define FALSE 0
#define TRUE 1
#define SIZE 26
#define UNKNOWN_VALUE -DBL_MAX

typedef enum Role{

	// There are two types of symbols to store, function and variable. 
	//For each we store diffrent information.
	PRE_DEF_FUNC,
	FULL_DEF_FUNC,
	VARIABLE,
	ARRAY,
	NONE
} Role;

// This struct holds token type and it's value. Helpful to check if function calls and array expressions is used properly.
typedef struct ValueType {
	int size;
	TOKEN_TYPE type;
	int value;
	struct ValueType* next;
}ValueType;

typedef struct Table {
	
	struct Table* father;
	struct TableEntry* symbolsArray[SIZE];
}Table;

typedef struct dimensionElem {

	int dimension;
	int size;
	struct dimensionElem* next;
}dimensionElem;

// TableEntry represent both variable or function of any kind in the same struct.
// In case of a variable the parmetersArray is a NULL 
typedef struct TableEntry {

	struct TableEntry* next;
	struct TableEntry* prev ;
	char* entryName; // name is the key
	TOKEN_TYPE type; // If the role is VARIABLE/Array type represnt the type of array it is. For in case of Role Func(any kind) this represnt the return type
	Role role;
	dimensionElem* dimensions; // Dimensions of array
	int usedEntry;
	int hasReturn;
	TOKEN_TYPE actualReturnType;
	// Attributes needed for a function
	struct TableEntry* funcSignature; // Parameter list for array definiton(pre or full)
	
} TableEntry;

Table* makeTable(Table* currTable);

Table* popTable(Table* currTable);

TableEntry* insert(Table* currTable, char* name);

TableEntry* createNewEntry(Table* currTable, char* name);

TableEntry* lookUp(Table* currTable, char* name); // Perform local search for name

TableEntry* find(Table* currTable, char* name); // Perform hierarchical search

void setType(TableEntry* idEntry, TOKEN_TYPE type);

TOKEN_TYPE getType(TableEntry* idEntry);

void setRole(TableEntry* idEntry, Role role);

Role getRole(TableEntry* idEntry);

void setFuncSignature(TableEntry* idEntry, TableEntry* parametersType);

TableEntry* getFuncSignature(TableEntry* idEntry);

void setDimensions(TableEntry* idEntry, dimensionElem* dimensionSizes);

dimensionElem* getDimensions(TableEntry* idEntry);

void freeTableMemory(Table* table);

int hashCode(char* name);

void setEntryName(TableEntry* currEntry, char* name);

char* getEntryName(TableEntry* currEntry);

void addDimension(TableEntry* tableEntry, int dimension);

TableEntry* getNewDummyEntry();

void copyEntryValuesToRealEntry(TableEntry* dummyEntry, TableEntry* realEntry);

void setUsedEntry(int useState, TableEntry *currEntry);

int getUsedEntry(TableEntry* currEntry);

void setTypeValue(ValueType*, TOKEN_TYPE , int);

const char* roleEnumToString(Role role);

void setActualReturnType(TableEntry*, TOKEN_TYPE);

TOKEN_TYPE getActualReturnType(TableEntry*);

ValueType* getDummyValueTypeItem();

ValueType* addItemToValueTypeList(ValueType* list, ValueType* item);

void freeValueTypeList(ValueType*);

TableEntry* addItemToList(TableEntry* list, TableEntry* item);

int getListSize(TableEntry* list);

int getValueTypeListSize(ValueType* list);

void setHasReturn(TableEntry*, int);

int getHasReturn(TableEntry*);

void freeEntry(TableEntry* entry);

#endif