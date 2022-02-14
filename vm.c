/*
	You can use these two print statements for the errors:
		printf("Virtual Machine Error: Stack Overflow Error\n");
		printf("Virtual Machine Error: Out of Bounds Access Error\n");
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define REG_FILE_SIZE 10
#define MAX_STACK_LENGTH 100

void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int *stack, int *RF)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t\t", line, opname, IR.r, IR.l, IR.m, PC, SP, BP);
	
	// print register file
	for (i = 0; i < REG_FILE_SIZE; i++)
		printf("%d ", RF[i]);
	printf("\n");
	
	// print stack
	printf("stack:\t");
	for (i = MAX_STACK_LENGTH - 1; i >= SP; i--)
		printf("%d ", stack[i]);
	printf("\n");
}

int base(int L, int BP, int *stack)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = stack[rtn];
		ctr--;
	}
	return rtn;
}

void execute_program(instruction *code, int printFlag)
{
	// Initialize Initial/Default Values
	int BP = MAX_STACK_LENGTH - 1;
	int SP = BP + 1;
	int PC = 0;
	instruction IR;

	// Initial stack and register file values are all zero.
	int stack[MAX_STACK_LENGTH] = {0};
	int reg[REG_FILE_SIZE] = {0};


	// keep this
	if (printFlag)
	{
		printf("\t\t\t\t\tPC\tSP\tBP\n");
		printf("Initial values:\t\t\t\t%d\t%d\t%d\n", PC, SP, BP);
	}

	int haltFlag = 0;

	// Continue looping through instructions until halt flag is reached (End of program).
	while (!haltFlag)
	{
		// Fetch set of instructions.
		IR.opcode = code[PC].opcode;
		IR.l = code[PC].l;
		IR.m = code[PC].m;
		IR.r = code[PC].r;


		// Increment for next set of instructions after execution.
		PC = PC + 1;

		// Execute set of instructions.
		switch (IR.opcode)
		{
			// LIT   
			case 1:
				reg[IR.r] = IR.m;
				break;

			// RET
			case 2:
				break;

			// LOD
			case 3: // NOT 100% SURE ON THIS YET
				if(!(base(IR.l,BP,stack)- IR.m) < 0 || !(base(IR.l,BP,stack) - IR.m) >= MAX_STACK_LENGTH)
					reg[IR.r] = stack[base(IR.l,BP,stack) - reg[IR.m]];
				break;

			// STO
			case 4: // NOT 100% SURE ON THIS YET
				if(!(base(IR.l,BP,stack) - IR.m) < 0 || !(base(IR.l,BP,stack) - IR.m) >= MAX_STACK_LENGTH)
					stack[base(IR.l,BP,stack) - reg[IR.m]] = reg[IR.r];
				break;
			// CAL
			case 5:
				stack[SP + 1] = base(IR.l,BP,stack);
				stack[SP + 2] = BP;
				stack[SP + 3] = PC;
				BP = SP + 1;
				PC = IR.m;
				break;

			// INC
			case 6:
				SP = SP + IR.m;
				break;

			// JMP
			case 7:
				PC = IR.m;
				break;

			// JPC
			case 8:
				if(reg[IR.r] == 0)
					PC = IR.m;
				break;

			// WRT
			case 9:
				printf("%d\n",reg[IR.r]);
				break;

			// RED
			case 10:
				break;

			// HAL   0 0 0   End of program (Set Halt flag to true).
			case 11:
				haltFlag = 1;
				break;

			// NEG
			case 12:
				reg[IR.r]--;
				break;

			// ADD
			case 13:
				reg[IR.r] = reg[IR.m] + reg[IR.l];
				break;

			// SUB
			case 14:
				reg[IR.r] = reg[IR.l] - reg[IR.m];
				break;

			// MUL
			case 15:
				reg[IR.r] = reg[IR.l] * reg[IR.m];
				break;

			// DIV
			case 16:
				break;

			// MOD
			case 17:
				break;

			// EQL
			case 18:
				break;

			// NEQ
			case 19:
				break;

			// LSS
			case 20:
				break;

			// LEQ
			case 21:
				break;

			// GTR
			case 22:
				break;

			// GEQ
			case 23:
				break;
		}

	}

}
