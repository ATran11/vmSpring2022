// Anthony Tran
// Gabriel De David
// Gani Begawala

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 200
#define MAX_SYMBOL_COUNT 50
#define MAX_REG_COUNT 10

// generated code
instruction *code;
int cIndex;

// symbol table
symbol *table;
int tIndex;

// global variables
int level;
int listIdx = 0;
int registerCounter;


void emit(int opname, int reg, int level, int mvalue);
void addToSymbolTable(int k, char n[], int s, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// Parse Functions
void program(lexeme* list);
void block(lexeme* list);
int varDeclaration(lexeme* list);
void procDeclaration(lexeme* list);
void statement(lexeme* list);
void condition(lexeme* list);
void expression(lexeme* list);
void term(lexeme* list);
void factor(lexeme* list);

instruction *parse(lexeme *list, int printTable, int printCode)
{
	registerCounter = -1;
	// set up program variables
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	cIndex = 0;
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0;

	// emit JMP-7 (M = 0, because we don’t know where main code starts)
	emit(7, 0, 0, 0);

	// add to symbol table
	addToSymbolTable(3, "main", 0, 0, 0, 0);

	level = -1;

	block(list);

	// Check for error 1
	if (list[listIdx].type != periodsym)
	{
		printparseerror(1);
	}

	// emit HALT
	emit(11, 0, 0, 0);

	// Fix M value of the initial JMP instruction
	code[0].m = table[0].addr;

	// Fix the M values of the CAL instructions
	for (int i = 0; i < cIndex; i++)
	{
		if (code[i].opcode == 5)
		{
			code[i].m = table[code[i].m].addr;
		}
	}
	
	// print off table and code
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();
	
	// mark the end of the code
	code[cIndex].opcode = -1;
	return code;
}


void block(lexeme* list)
{
	// Increment a level
	level++;
	
	// Store tIndx -1 in a variable
	int procedureidx = tIndex - 1
	
	// Store var - declaration() in variable x to call later on 
	int x = varDeclaration(list);
	
	// procedure declaration
	procDeclaration(list);
	
	//Add it to the array
	table[procedureidx].addr = cIndex;
	
	// Call emit
	emit(6, 0, 0, x);
		
	statement(list);
	
	mark();
	
	//Decrement a level
	level--;
}

int varDeclaration(lexeme* list)
{
	int memSize = 3;

	char symbolName[20] = "\0";

	int arrSize = 0;

	if (list[listIdx].type == varsym)
	{
		
		do
		{
			listIdx++;

			if (list[listIdx].type != identsym)
			{
				printparseerror(2);
				return 0;
			}

			if (multipledeclarationcheck(list[listIdx].name) != -1)
			{
				printparseerror(3);
				return 0;
			}

			strcpy(list[listIdx].name, symbolName);

			listIdx++;

			if (list[listIdx].type == lbracketsym)
			{
				listIdx++;

				if (list[listIdx].type != numbersym || list[listIdx].value == 0)
				{
					printparseerror(4);
				}

				arrSize = list[listIdx].value;

				listIdx++;

				if (list[listIdx].type == multsym || list[listIdx].type == divsym || list[listIdx].type == modsym || list[listIdx].type == addsym || list[listIdx].type == subsym)
				{
					printparseerror(4);
				}
				else if (list[listIdx].type != rbracketsym)
				{
					printparseerror(5);
				}

				listIdx++;

				addToSymbolTable(2, symbolName, arrSize, level, memSize, 0);

				memSize += arrSize;
			}
			else
			{
				// This is for a var.

				addToSymbolTable(1, symbolName, 0, level, memSize, 0);

				memSize++;
			}
		} while (list[listIdx].type == commasym);

		if (list[listIdx].type == identsym)
		{
			printparseerror(6);
			return 0;
		}

		else if (list[listIdx].type != semicolonsym)
		{
			printparseerror(7);
		}

		listIdx++;
	}
	else
	{
		return memSize;
	}

	return 0;
}

void procDeclaration(lexeme* list)
{
	char symbolName[20] = "\0";

	while (list[listIdx].type == procsym)
	{
		listIdx++;

		if (list[listIdx].type != identsym)
		{
			printparseerror(2);
			return;
		}

		else if (multipledeclarationcheck(list[listIdx].name) != -1)
		{
			printparseerror(3);
			return;
		}

		strcpy(list[listIdx].name, symbolName);
		listIdx++;

		if (list[listIdx].type != semicolonsym)
		{
			printparseerror(8);
			return;
		}

		listIdx++;
		addToSymbolTable(3, symbolName, 0, level, 0, 0);
		block(list);

		if (list[listIdx].type != semicolonsym)
		{
			printparseerror(7);
			return;
		}

		listIdx++;

		// emit RET
		emit(2, 0, 0, 0); 

	}
}

void statement(lexeme* list)
{
	int symbolName;
	int arrayIdxReg;
	int varLocReg;
	int jpcIdx;
	int jmpIdx;
	int loopIdx;

	//assignment
	// check for assignment statement
	if(){
		symbolName = list[listIdx].name;
		listIdx++;
		if(list[listIdx].type == lbracketsym) {
			listIdx++;
			int sym  = findsymbol(symbolName, 2);
			if(sym == -1) {
				if(findsymbol(symbolName,1) != -1) {
					printparseerror(11);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
			}
			expression(list);
			arrayIdxReg = registerCounter;
			if(list[listIdx].type != rbracketsym) {
				printparseerror(5);
				return 0;
			}
			listIdx++;
			if(list[listIdx].type != assignsym) {
				printparseerror(13);
				return 0;
			}
			listIdx++;
			expression(list);
			registerCounter++;
			if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
			}
			emit (1, registerCounter, table[sym].addr); //LIT
			emit (13, arrayIdxReg, arrayIdxReg, registerCounter); //ADD
			registerCounter--;
			emit (4, registerCounter, level - table[sym].level, arrayIdxReg); //STO
			registerCounter -= 2;
				
		}
		else {
			int sym = findsymbol(symbolName, 1);
			if(sym == -1) {
				if(findsymbol(symbolName,2) != -1) {
					printparseerror(12);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
				registerCounter++;
				if(registerCounter >= 10) {
					printparseerror(14);
					return 0;
				}
			}
			emit (1, registerCounter,0, table[sym].addr); //LIT
			varLocReg = registerCounter;
			if(list[listIdx].type != assignsym) {
				printparseerror(13);
				return 0;
			}
			listIdx++;
			expression(list);
			emit (4, registerCounter, level - table[sym].level, varLocReg); //STO
			registerCounter -= 2;
		}
	}
	//call
	//check for call-statement
	if(){	
		listIdx++;
		if(list[listIdx].type != identsym){
			printparseerror(15);
			return 0;
		}
		sym = findsymbol(list[listIdx].name, 3);
		if(sym == -1){
			if(findsymbol(list[listIdx].name, 1) != -1 || findsymbol(list[listIdx].name, 2) != -1) {
				printparseerror(15);
				return 0;
			}
			else {
				printparseerror(10);
				return 0;
			}	
		}
		emit (15, 0, level - table[sym].level, sym); //CAL
		listIdx++;
	}
	
	//begin-end
	//check for begin-end statement
	if(){
		do{
			listIdx;
			statement(list);
		}
		while(list[listIdx].type == semicolonsym);
		if(list[listIdx].type != endsym) {
			if(list[listIdx].type == identsym || list[listIdx].type == callsym || list[listIdx].type == beginsym
				|| list[listIdx].type == ifsym || list[listIdx].type == dosym || list[listIdx].type == readsym || list[listIdx].type == writesym) {
					printparseerror(16);
					return 0;
			}
			else {
				printparseerror(17);
				return 0;
			}
		}
		listIdx++;
	}
	//if
	// check for if-statement
	if(){
		listIdx++;
		condition(list);
		jpcIdx = cIndex;
		emit (8, registerCounter, 0, 0); //JPC
		registerCounter--;
		if(list[listIdx].type != questionsym) {
			printparseerror(18);
			return 0;
		}
		listIdx++;
		statement(list);
		if(list[listIdx].type != colonsym){
			listIdx++;
			jmpIdx = cIndex;
			emit (7, 0, 0, 0); // JMP
			code[jpcIdx].m = cIndex;
			statement(list);
			code[jmpIdx].m = cIndex;
		}
		else {
			code[jpcIdx].m = cIndex;
		}
	}

	//do-while
	// check for do-while statement
	if(){
		listIdx++;
		loopIdx = cIndex;
		statement(list);
		if(list[listIdx].type != whilesym){
			printparseerror(19);
			return 0;
		}
		listIdx++;
		condition(list);
		registerCounter++;
		if(registerCounter >= 10){
			printparseerror(14);
		}
		emit (1, registerCounter, 0, 0); //LIT
		emit (18, registerCounter - 1, registerCounter - 1, registerCounter); //EQL
		registerCounter--;
		emit (8, registerCounter, 0, loopIdx); //JPC
		registerCounter--;
	}

	//read
	// check for read statement
	if(){
		listIdx++;

		if(list[listIdx].type != identsym) {
			printparseerror(20);
			return 0;
		}

		symbolName = list[listIdx].name;
		listIdx++;	

		if(list[listIdx].type == lbracketsym) {
			listIdx++;
			int sym  = findsymbol(symbolName, 2);
			if(sym == -1) {
				if(findsymbol(symbolName,1) != -1) {
					printparseerror(11);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
			}
			expression(list);
			arrayIdxReg = registerCounter;
			if(list[listIdx].type != rbracketsym) {
				printparseerror(5);
				return 0;
			}
			listIdx++;
			registerCounter++;
			if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
			}
			emit (10, registerCounter, 0, 0); //RED
			registerCounter++;
			if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
			}
			emit LIT(1, registerCounter, 0, table[sym].addr); //LIT
			emit ADD(13, arrayIdxReg, arrayIdxReg, registerCounter); //ADD
			registerCounter--;
			emit STO(4, registerCounter, level - table[sym].level, arrayIdxReg); //STO
			registerCounter -= 2;
			
		}
		else {
			int sym = findsymbol(symbolName, 1);
			if(sym == -1)
				if(findsymbol(symbolName,2) != -1) {
					printparseerror(12);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
				registerCounter++;
				if(registerCounter >= 10) {
					printparseerror(14);
					return 0;
				}
				emit (1, registerCounter, 0, table[sym].addr); // LIT
				varLocReg = registerCounter;
				registerCounter++;
				if(registerCounter >= 10) {
					printparseerror(14);
					return 0;
				}
				emit RED(10, registerCounter, 0 ,0); //RED
				emit STO(4, registerCounter, level - table[sym].level, varLocReg); //STO
				registerCounter--;
		}
	}
	//write
	//Check for write statements
	if(){
		listIdx++;
		expression(list);
		emit WRT(9, registerCounter, 0, 0);//WRT
		registerCounter--;
	}
}

void condition(lexeme* list)
{
     expression(list);
     if (list[listIdx] == eqlsym)
     {
	listIdx++;
	expression(list);
	emit(18, registerCounter - 1, registerCounter - 1, registerCounter); // emit EQL
	registerCounter--;     
     }   
     else if (list[listIdx] == neqsym)
     {
	listIdx++;
	expression(list);
	emit(19, registerCounter - 1, registerCounter - 1, registerCounter); // emit NEQ
	registerCounter--;     
     } 
	else if (list[listIdx] == lsssym)
     {
	listIdx++;
	expression(list);
	emit(20, registerCounter - 1, registerCounter - 1, registerCounter); // emit LSS
	registerCounter--;     
     } 
	else if (list[listIdx] == leqsym)
     {
	listIdx++;
	expression(list);
	emit(21, registerCounter - 1, registerCounter - 1, registerCounter); // emit LEQ
	registerCounter--;     
     } 
	else if (list[listIdx] == gtrsym)
     {
	listIdx++;
	expression(list);
	emit(22, registerCounter - 1, registerCounter - 1, registerCounter); // emit GTR
	registerCounter--;     
     } 
	else if (list[listIdx] == geqsym)
     {
	listIdx++;
	expression(list);
	emit(23, registerCounter - 1, registerCounter - 1, registerCounter); // emit GEQ
	registerCounter--;     
     } 
	else
	{
		printparseerror(21);
		return 0;
	}	
}

void expression(lexeme* list)
{
   if(list[listIdx] == subsym)
   {
	listInd++;
	term(list);
	emit(12, registerCounter, 0, registerCounter); // emit NEG
	while(list[listIdx] == addsym || list[listIdx] == subsym)
	{
		if(list[listidx] == addsym)
		{
			listIdx++;
			term(list);
			emit(13, registerCounter - 1, registerCounter - 1, registerCounter); // emit ADD
			registerCounter--;
		}
		else
		{
			listIdx++;
			term(list);
			emit(14, registerCounter - 1, registerCounter - 1, registerCounter); // emit SUB
			registerCounter--;
		}	
	}	
   }
   else
   {
	term(list);
	while(list[listIdx] == addsym || list[listIdx] == subsym)
	{
		if(list[listidx] == addsym)
		{
			listIdx++;
			term(list);
			emit(13, registerCounter - 1, registerCounter - 1, registerCounter); // emit ADD
			registerCounter--;
		}
		else
		{
			listIdx++;
			term(list);
			emit(14, registerCounter - 1, registerCounter - 1, registerCounter); // emit SUB
			registerCounter--;
		}	
	}	
	   
   }
  if(list[listIdx] == lparenthesissym || list[listIdx] == identsym || list[listIdx] == numbersym)
  {
	  printparseerror(22);	   
	  return 0;
  }
}

void term(lexeme* list)
{
   factor(list);
   while(list[listIdx] == multsym || list[listIdx] == divsym ||	list[listIdx] == modsym)
   {
	if(list[listIdx] == multsym)
	{
	 listIdx++;
	 factor(list);
	 emit(15, registerCounter -1, registerCounter -1, registerCounter); // emit MUL
	 registerCounter--;
	}
	else if(list[listIdx] == divsym)
	{
	 listIdx++;
	 factor(list);
	 emit(16, registerCounter -1, registerCounter -1, registerCounter);  // emit DIV
	 registerCounter--;
	}
	else
	{
	 listIdx++;
	 factor(list);
	 emit(17, registerCounter -1, registerCounter -1, registerCounter); // emit MOD
	 registerCounter--;
	}
   }	   
}

void factor(lexeme* list)
{
	int symbolName;
	int arrayIdxReg;
	int varLocReg;

	if(list[listIdx].type == identsym) {
		symbolName = list[listIdx].name;
		listIdx++;
		if(list[listIdx] == lbracketsym) {
			listIdx++;
			int sym  = findsymbol(symbolName, 2);
			if(sym == -1) {
				if(findsymbol(symbolName,1) != -1) {
					printparseerror(11);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
			}
			expression(list);
			arrayIdxReg = registerCounter;
			if(list[listIdx].type != rbracketsym) {
				printparseerror(5);
				return 0;
			}
			listIdx++;
			registerCounter++;
			if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
			}
			emit (1, registerCounter, 0, table[sym].addr); //LIT
			emit (13, arrayIdxReg, arrayIdxReg, registerCounter); //ADD
			registerCounter--;
			emit (3, registerCounter, level - table[sym].level, arrayIdxReg); //LOD
		}
		else {
			int sym = findsymbol(symbolName, 1);
			if(sym == -1) {
				if(findsymbol(symbolName,2) != -1) {
					printparseerror(12);
					return 0;
				}
				if(findsymbol(symbolName,3) != -1) {
					printparseerror(9);
					return 0;
				}
				else {
					printparseerror(10);
					return 0;
				}
			}
			registerCounter++;
			if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
			}
			emit (1, registerCounter, 0, table[sym].addr); //LIT
			varLocReg = registerCounter;
			emit (3, registerCounter, level - table[sym].level, varLocReg); //LOD
		}
	}
	else if(list[listIdx].type == numbersym) {
		registerCounter++;
		if(registerCounter >= 10) {
				printparseerror(14);
				return 0;
		}
		emit (1, registerCounter, 0, list[listIdx].value)
		listIdx++;
	}
	else if(list[listIdx].type == lparenthesissym) {
		listIdx++;
		expression(list);
		if(list[listIdx].type != rparenthesissym) {
			printparseerror(23); 
			return 0;
		}
		listIdx++;
	}
	else {
		printparseerror(24);
		return 0;
	}
}

void emit(int opname, int reg, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].r = reg;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int s, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].size = s;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Symbol names must be identifiers\n");
			break;
		case 3:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 4:
			printf("Parser Error: Array sizes must be given as a single, nonzero number\n");
			break;
		case 5:
			printf("Parser Error: [ must be followed by ]\n");
			break;
		case 6:
			printf("Parser Error: Multiple symbols in var declaration must be separated by commas\n");
			break;
		case 7:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 8:
			printf("Parser Error: Procedure declarations should contain a semicolon before the body of the procedure begins\n");
			break;
		case 9:
			printf("Parser Error: Procedures may not be assigned to, read, or used in arithmetic\n");
			break;
		case 10:
			printf("Parser Error: Undeclared identifier\n");
			break;
		case 11:
			printf("Parser Error: Variables cannot be indexed\n");
			break;
		case 12:
			printf("Parserr Error: Arrays must be indexed\n");
			break;
		case 13:
			printf("Parser Error: Assignment operator missing\n");
			break;
		case 14:
			printf("Parser Error: Register Overflow Error\n");
			break;
		case 15:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 16:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 17:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 18:
			printf("Parser Error: if must be followed by ?\n");
			break;
		case 19:
			printf("Parser Error: do must be followed by while\n");
			break;
		case 20:
			printf("Parser Error: read must be followed by a var or array identifier\n");
			break;
		case 21:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 22:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 23:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 24:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, and variables\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Size | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %4d | %5d | %5d\n", table[i].kind, table[i].name, table[i].size, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tR\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				printf("RET\t");
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("WRT\t");
				break;
			case 10:
				printf("RED\t");
				break;
			case 11:
				printf("HLT\t");
				break;
			case 12:
				printf("NEG\t");
				break;
			case 13:
				printf("ADD\t");
				break;
			case 14:
				printf("SUB\t");
				break;
			case 15:
				printf("MUL\t");
				break;
			case 16:
				printf("DIV\t");
				break;
			case 17:
				printf("MOD\t");
				break;
			case 18:
				printf("EQL\t");
				break;
			case 19:
				printf("NEQ\t");
				break;
			case 20:
				printf("LSS\t");
				break;
			case 21:
				printf("LEQ\t");
				break;
			case 22:
				printf("GTR\t");
				break;
			case 23:
				printf("GEQ\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\t%d\n", code[i].r, code[i].l, code[i].m);
	}
	
	if (table != NULL)
		free(table);
}
