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
int regCounter = 0;


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
	// set up program variables
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	cIndex = 0;
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0;
	
	regCounter = -1;

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
		return NULL;
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
	int procedureIdx = tIndex - 1;

	// Store var - declaration() in variable x to call later on
	int x = varDeclaration(list);

	// procedure declaration
	procDeclaration(list);

	// Add it to the array
	table[procedureIdx].addr = cIndex;


	// Call emit
	emit(6, 0, 0, x);

	statement(list);

	mark();

	// Decrement a level
	level--;
}


int varDeclaration(lexeme* list)
{
	int memSize = 3;

	char symbolName[20];

	int arrSize;

	
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

			strcpy(symbolName, list[listIdx].name);

			listIdx++;

			if (list[listIdx].type == lbracketsym)
			{
				listIdx++;

				if (list[listIdx].type != numbersym || list[listIdx].value == 0)
				{
					printparseerror(4);
					return 0;
				}

				arrSize = list[listIdx].value;

				listIdx++;

				if (list[listIdx].type == multsym || list[listIdx].type == divsym || list[listIdx].type == modsym 
					|| list[listIdx].type == addsym || list[listIdx].type == subsym)
				{
					printparseerror(4);
					return 0;
				}
				else if (list[listIdx].type != rbracketsym)
				{
					printparseerror(5);
					return 0;
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
			return 0;
		}

		listIdx++;

		return memSize;
	}

	return memSize;
}


void procDeclaration(lexeme* list)
{
	char symbolName[20];

	while (list[listIdx].type == procsym)
	{
		listIdx++;

		if (list[listIdx].type != identsym)
		{
			printparseerror(2);
			return;
		}
		
		if (multipledeclarationcheck(list[listIdx].name) != -1)
		{
			printparseerror(3);
			return;
		}

		strcpy(symbolName, list[listIdx].name);
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
	char symbolName[20];

	int symIdx;

	int arrayIdxReg;

	int varLocationReg;

	int jmpIdx;

	int jpcIdx;

	int loopIdx;

	// Assignment
	if(list[listIdx].type == identsym)
	{
		
		strcpy(symbolName, list[listIdx].name);
		listIdx++;

		if(list[listIdx].type == lbracketsym)
		{
			listIdx++;
			symIdx = findsymbol(symbolName, 2);
			if (symIdx == -1)
			{
				if (findsymbol(symbolName, 1) != -1)
				{
					printparseerror(11);
					return;
				}
				else if (findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			expression(list);
			arrayIdxReg = regCounter;
			if (list[listIdx].type != rbracketsym)
			{
				printparseerror(5);
				return;
			}

			listIdx++;

			if (list[listIdx].type != assignsym)
			{
				printparseerror(13);
				return;
			}

			listIdx++;

			expression(list);

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1, regCounter, 0, table[symIdx].addr);

			// ADD
			emit(13, arrayIdxReg, arrayIdxReg, regCounter);

			regCounter--;

			// STO
			emit(4, regCounter, level - table[symIdx].level, arrayIdxReg);

			regCounter -= 2;
		}
		else
		{
			symIdx = findsymbol(symbolName, 1);
			if (symIdx == -1)
			{
				if (findsymbol(symbolName, 2) != -1)
				{
					printparseerror(12);
					return;
				}
				else if(findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1,regCounter, 0, table[symIdx].addr);

			varLocationReg = regCounter;

			if (list[listIdx].type != assignsym)
			{
				printparseerror(13);
				return;
			}

			listIdx++;

			expression(list);

			// STO
			emit(4, regCounter, level - table[symIdx].level, varLocationReg);

			regCounter -= 2;
		}

		return;
	}

	// Call
	if (list[listIdx].type == callsym)
	{
		listIdx++;

		if (list[listIdx].type != identsym)
		{
			printparseerror(15);
			return;
		}

		symIdx = findsymbol(list[listIdx].name, 3);

		if(symIdx == -1)
		{
			if(findsymbol(list[listIdx].name, 1) != -1 || findsymbol(list[listIdx].name, 2) != -1)
			{
				printparseerror(15);
				return;
			}
			else
			{
				printparseerror(10);
				return;
			}
		}

		// CAL
		emit(5, 0, level-table[symIdx].level, symIdx);

		listIdx++;

		return;
	}

	// Begin-end
	if (list[listIdx].type == beginsym)
	{
		do
		{
			listIdx++;

			statement(list);

		} while (list[listIdx].type == semicolonsym);

		if (list[listIdx].type != endsym)
		{
			token_type currToken = list[listIdx].type;

			if(currToken == identsym || currToken == callsym || currToken == beginsym || 
			   currToken == ifsym || currToken == dosym || currToken == readsym || currToken == writesym)
			{
			   printparseerror(16);
			   return;
		    }
			else
			{
				printparseerror(17);
				return;
			}
		}

		listIdx++;

		return;
	}

	// If
	if (list[listIdx].type == ifsym)
	{
		listIdx++;

		condition(list);

		jpcIdx = cIndex;

		// JPC
		emit(8, regCounter, 0, 0);

		regCounter--;

		if (list[listIdx].type != questionsym)
		{
			printparseerror(18);
			return;
		}

		listIdx++;

		statement(list);

		if (list[listIdx].type == colonsym)
		{
			listIdx++;

			jmpIdx = cIndex;

			// JMP
			emit(7,0,0,0);

			code[jpcIdx].m = cIndex;

			statement(list);

			code[jmpIdx].m = cIndex;
		}
		else
		{
			code[jpcIdx].m = cIndex;
		}
		return;
	}

	// Do-while
	if(list[listIdx].type == dosym)
	{
		listIdx++;

		loopIdx = cIndex;

		statement(list);

		if (list[listIdx].type != whilesym)
		{
			printparseerror(19);
			return;
		}

		listIdx++;

		condition(list);

		regCounter++;
		if (regCounter >= 10)
		{
			printparseerror(14);
			return;
		}

		// LIT
		emit(1, regCounter, 0, 0);

		// EQL
		emit(18, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;

		// JPC
		emit(8, regCounter, 0, loopIdx);

		return;
	}

	// Read
	if (list[listIdx].type == readsym)
	{
		listIdx++;

		if (list[listIdx].type != identsym)
		{
			printparseerror(20);
			return;
		}

		strcpy(symbolName, list[listIdx].name);

		listIdx++;

		if (list[listIdx].type == lbracketsym)
		{
			listIdx++;

			symIdx = findsymbol(symbolName, 2);

			if (symIdx == -1)
			{
				if (findsymbol(symbolName, 1) != -1)
				{
					printparseerror(11);
					return;
				}
				else if (findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			expression(list);

			arrayIdxReg = regCounter;

			if (list[listIdx].type != rbracketsym)
			{
				printparseerror(5);
				return;
			}

			listIdx++;

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// RED
			emit(10, regCounter, 0, 0);

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1, regCounter, 0, table[symIdx].addr);

			// ADD
			emit(13, arrayIdxReg, arrayIdxReg, regCounter);

			regCounter--;

			// STO
			emit(4, regCounter, level - table[symIdx].level, arrayIdxReg);

			regCounter -= 2;
		}
		else
		{
			symIdx = findsymbol(symbolName, 1);

			if (symIdx == -1)
			{
				if (findsymbol(symbolName, 2) != -1)
				{
					printparseerror(12);
					return;
				}
				else if (findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1, regCounter, 0, table[symIdx].addr);

			varLocationReg = regCounter;

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// RED
			emit(10,regCounter, 0,0);

			// STO
			emit(4, regCounter, level - table[symIdx].level, varLocationReg);

			regCounter -= 2;
		}

		return;
	}

	// Write
	if(list[listIdx].type == writesym)
	{
		listIdx++;

		expression(list);

		// WRT
		emit(9, regCounter, 0, 0);

		regCounter--;

		return;
	}	
}


void condition(lexeme* list)
{
	expression(list);

	if (list[listIdx].type == eqlsym)
	{
		listIdx++;

		expression(list);

		// EQL
		emit(18, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else if (list[listIdx].type == neqsym)
	{
		listIdx++;

		expression(list);

		// NEQ
		emit(19, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else if (list[listIdx].type == lsssym)
	{
		listIdx++;

		expression(list);

		// LSS
		emit(20, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else if (list[listIdx].type == leqsym)
	{
		listIdx++;

		expression(list);

		// LEQ
		emit(21, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else if (list[listIdx].type == gtrsym)
	{
		listIdx++;

		expression(list);

		// GTR
		emit(22, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else if (list[listIdx].type == geqsym)
	{
		listIdx++;

		expression(list);

		// GEQ
		emit(23, regCounter - 1, regCounter - 1, regCounter);

		regCounter--;
	}
	else
	{
		printparseerror(21);
		return;
	}
}


void expression(lexeme* list)
{
	if (list[listIdx].type == subsym)
	{
		listIdx++;

		term(list);

		// NEG
		emit(12, regCounter, 0, regCounter);

		while (list[listIdx].type == addsym || list[listIdx].type == subsym)
		{
			if (list[listIdx].type == addsym)
			{
				listIdx++;

				term(list);

				// ADD
				emit(13, regCounter - 1, regCounter - 1, regCounter);

				regCounter--;
			}
			else
			{
				listIdx++;

				term(list);

				// SUB
				emit(14, regCounter - 1, regCounter - 1, regCounter);

				regCounter--;
			}
		}
	}
	else
	{
		term(list);

		while (list[listIdx].type == addsym || list[listIdx].type == subsym)
		{
			if (list[listIdx].type == addsym)
			{
				listIdx++;

				term(list);

				// ADD
				emit(13, regCounter - 1, regCounter - 1, regCounter);

				regCounter--;
			}
			else
			{
				listIdx++;

				term(list);

				// SUB
				emit(14, regCounter - 1, regCounter - 1, regCounter);

				regCounter--;
			}
		}
	}

	if (list[listIdx].type == lparenthesissym || list[listIdx].type == identsym || list[listIdx].type == numbersym)
	{
		printparseerror(22);
		return;
	}
}

void term(lexeme* list)
{
	factor(list);

	while (list[listIdx].type == multsym || list[listIdx].type == divsym || list[listIdx].type == modsym)
	{
		if (list[listIdx].type == multsym)
		{
			listIdx++;

			factor(list);

			// MUL
			emit(15, regCounter - 1, regCounter - 1, regCounter);

			regCounter--;
		}
		else if (list[listIdx].type == divsym)
		{
			listIdx++;

			factor(list);

			// DIV
			emit(16, regCounter - 1, regCounter - 1, regCounter);

			regCounter--;
		}
		else 
		{
			listIdx++;

			factor(list);

			// MOD
			emit(17, regCounter - 1, regCounter - 1, regCounter);

			regCounter--;
		}
	}
}


void factor(lexeme* list)
{
	char symbolName[20] = "\0";

	int symidx;

	int arrayIdxReg;

	int varLocationReg;
	
	if (list[listIdx].type == identsym)
	{
		strcpy(symbolName, list[listIdx].name);

		listIdx++;

		if (list[listIdx].type == lbracketsym)
		{
			listIdx++;

			symidx = findsymbol(symbolName, 2);

			if (symidx == -1)
			{
				if (findsymbol(symbolName, 1) != -1)
				{
					printparseerror(11);
					return;
				}
				else if (findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			expression(list);

			arrayIdxReg = regCounter;

			if (list[listIdx].type != rbracketsym)
			{
				printparseerror(5);
				return;
			}

			listIdx++;

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1, regCounter, 0, table[symidx].addr);

			// ADD
			emit(13, arrayIdxReg, arrayIdxReg, regCounter);

			regCounter--;

			// LOD
			emit(3, regCounter, level - table[symidx].level, arrayIdxReg);
		}
		else
		{
			symidx = findsymbol(symbolName, 1);

			if (symidx == -1)
			{
				if (findsymbol(symbolName, 2) != -1)
				{
					printparseerror(12);
					return;
				}
				else if (findsymbol(symbolName, 3) != -1)
				{
					printparseerror(9);
					return;
				}
				else
				{
					printparseerror(10);
					return;
				}
			}

			regCounter++;
			if (regCounter >= 10)
			{
				printparseerror(14);
				return;
			}

			// LIT
			emit(1, regCounter, 0, table[symidx].addr);

			varLocationReg = regCounter;

			// LOD
			emit(3, regCounter, level - table[symidx].level, varLocationReg);
		}
	}
	else if (list[listIdx].type == numbersym)
	{
		regCounter++;
		if (regCounter >= 10)
		{
			printparseerror(14);
			return;
		}

		// LIT
		emit(1, regCounter, 0, list[listIdx].value);

		listIdx++;
	}
	else if (list[listIdx].type == lparenthesissym)
	{
		listIdx++;

		expression(list);

		if (list[listIdx].type != rparenthesissym)
		{
			printparseerror(23);
			return;
		}

		listIdx++;
	}
	else
	{
		printparseerror(24);
		return;
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

	exit(0);
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
