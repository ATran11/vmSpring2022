#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

int alphatoken(char * input, int inputIndex);
int numbertoken(char * input, int inputIndex);
int symboltoken(char * input, int inputIndex);
int reservedcheck(char *buffer);
void printlexerror(int type);
void printtokens();

// Check for word.
int alphatoken(char * input, int inputIndex)
{
	// Create temp array to hold letters.
	char buffer[MAX_IDENT_LEN + 1] = "\0";
	strncat(buffer, &input[inputIndex], 1);
	
  	// As long as the input is a letter or number we add to temp array and increment.
	while (strlen(buffer) < MAX_IDENT_LEN + 1)
	{
		if (isalnum(input[inputIndex + 1]))
		{
			inputIndex++;
			strncat(buffer, &input[inputIndex], 1);
			continue;
		}
		
		break;
	}
	
	// Check for identifier length (3. Identfier Length).
	if (strlen(buffer) == MAX_IDENT_LEN + 1)
		return -3;
	
	// Check if it's a reserved word otherwise it's an identifier.
	if (reservedcheck(buffer) == 0)
	{
		list[lex_index].type = identsym;
		strcpy(list[lex_index++].name, buffer);
	}
 
	return ++inputIndex;
}

// Check for reserved words.
int reservedcheck(char *buffer)
{
	if(strcmp(buffer, "var") == 0)
   {
       list[lex_index++].type = varsym;
   }
   else if(strcmp(buffer, "procedure") == 0)
   {
        list[lex_index++].type = procsym;
   }
   else if(strcmp(buffer, "call") == 0)
   {
        list[lex_index++].type = callsym;
   }
   else if(strcmp(buffer, "begin") == 0)
   {
       list[lex_index++].type = beginsym;
   }
   else if(strcmp(buffer, "end") == 0)
   {
        list[lex_index++].type = endsym;
   }
   else if(strcmp(buffer, "if") == 0)
   {
        list[lex_index++].type = ifsym;
   }
   else if(strcmp(buffer, "do") == 0)
   {
        list[lex_index++].type = dosym;
   }
   else if(strcmp(buffer, "while") == 0)
   {
        list[lex_index++].type = whilesym;
   }
    else if(strcmp(buffer, "read") == 0)
   {
        list[lex_index++].type = readsym;
   }
    else if(strcmp(buffer, "write") == 0)
   {
        list[lex_index++].type = writesym;
   }
   else
   {
        return 0;
   }
	
	return 1;
}

// Check for number.
int numbertoken(char * input, int inputIndex)
{
	// Create temp array to hold numbers.
	char buffer[MAX_NUMBER_LEN + 1] = "\0";
	strncat(buffer, &input[inputIndex], 1);
	
  	// As long as the input is a number we add to temp array and increment.
	while (strlen(buffer) < MAX_NUMBER_LEN + 1)
	{
		// Check if next current Character is a letter then its a INVALID IDENTIFIER ERROR (1. Invalid Identifier)
		if (isalpha(input[inputIndex + 1]))
			return -1;
		// If it's a number we add to buffer and continue.
		else if (isdigit(input[inputIndex + 1]))
		{
			inputIndex++;
			strncat(buffer, &input[inputIndex], 1);
			continue;
		}

		break;
	}
	
	// If we break from previous while loop, we need to check why we broke out to print the correct error message 
	// Check if our numCount == 6 then its a NUMBER LENGTH ERROR (2. Number Length)
	if (strlen(buffer) == MAX_NUMBER_LEN + 1)
		return -2;
	
	list[lex_index].type = numbersym;
  	strcpy(list[lex_index].name, "numbersym");
	list[lex_index++].value = atoi(buffer);
	return ++inputIndex;
}

// Check for special symbols.
int symboltoken(char * input, int inputIndex)
{

  
	switch (input[inputIndex])
	{
		case '.':
			list[lex_index++].type = periodsym;
			return ++inputIndex;
		case '[':
			list[lex_index++].type = lbracketsym;
			return ++inputIndex;
		case ']':
			list[lex_index++].type = rbracketsym;
			return ++inputIndex;
		case ',':
			list[lex_index++].type = commasym;
			return ++inputIndex;
		case ';':
			list[lex_index++].type = semicolonsym;
			return ++inputIndex;
		case ':':
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = assignsym;
				return inputIndex + 2;
			}
      		
			list[lex_index++].type = colonsym;
			return ++inputIndex;
		case '?':
			list[lex_index++].type = questionsym;
			return ++inputIndex;
		case '(':
			list[lex_index++].type = lparenthesissym;
			return ++inputIndex;
		case ')':
			list[lex_index++].type = rparenthesissym;
			return ++inputIndex;
		case '=':
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = eqlsym;
				return inputIndex + 2;
			}
			// '=' is not a valid symbol (4. Invalid Symbol)
			return -4;
		case '<':
			if (input[inputIndex + 1] == '>')
			{
				list[lex_index++].type = neqsym;
				return inputIndex + 2;
			}
			
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = leqsym;
				return inputIndex + 2;
			}
      		
			list[lex_index++].type = lsssym;
			return ++inputIndex;
		case '>':
			if (input[inputIndex + 1] == '=')
			{
        		
				list[lex_index++].type = geqsym;
				return inputIndex + 2;
			}
      		
			list[lex_index++].type = gtrsym;
			return ++inputIndex;
		case '+':
      		
			list[lex_index++].type = addsym;
			return ++inputIndex;
		case '-':
      		
			list[lex_index++].type = subsym;
			return ++inputIndex;
		case '*':
			list[lex_index++].type = multsym;
			return ++inputIndex;
		case '/':
			// Check for comment.
			if (input[inputIndex + 1] == '/')
			{
				++inputIndex;
        		
				while (input[inputIndex] != '\n' && input[inputIndex] != '\0')
				{
					++inputIndex;
				}
        
				return inputIndex;
			}
			// Otherwise it's a divide symbol.
			list[lex_index++].type = divsym;
			return ++inputIndex;
		case '%':
      	
			list[lex_index++].type = modsym;
			return ++inputIndex;
	}
	
	// If none of the symbols match it is an invalid symbol (4. Invalid Symbol).
  	return -4;
}

lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;
	
	int inputIndex = 0;
	
	// Loop to go through the input file as long as we're not at EOF and there are no errors.
	while (input[inputIndex] != '\0' && inputIndex >= 0)
	{
		// Check for whitespace and space character.
	  	if (iscntrl(input[inputIndex]) || isspace(input[inputIndex]))
		{
			inputIndex++;
			continue;
		}
		// Check if current Character in the input is a number.
		else if (isdigit(input[inputIndex]))
		{
			inputIndex = numbertoken(input, inputIndex);
			
			// Handle case for (2. Number Length).
			if (inputIndex == -2)
			{
        		printlexerror(2);
			}
			
			// Handle case for (1. Invalid Identifier).
			else if (inputIndex == -1)
			{
			  	printlexerror(1);	
			}

		}
		// Check if current Character in the input is a letter.
		else if (isalpha(input[inputIndex]))
		{
			inputIndex = alphatoken(input, inputIndex);
			
			// Handle case for (3. Identifier Length)
			if (inputIndex == -3)
			{
			  	printlexerror(3);
			}
			
		}
		// Otherwise current Character is a symbol.
		else
		{
			inputIndex = symboltoken(input, inputIndex);
			
			// Handle case for (4. Invalid Symbol)
			if (inputIndex == -4)
			{
			  	printlexerror(4);
			}
		}
	}
  
	if (printFlag)
		printtokens();
	list[lex_index++].type = -1;
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case lbracketsym:
				printf("%11s\t%d", "[", lbracketsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case rbracketsym:
				printf("%11s\t%d", "]", rbracketsym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case questionsym:
				printf("%11s\t%d", "?", questionsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case colonsym:
				printf("%11s\t%d", ":", colonsym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case lparenthesissym:
				printf("%11s\t%d", "(", lparenthesissym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case rparenthesissym:
				printf("%11s\t%d", ")", rparenthesissym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			default:
				printf("%11s\t%s", "err", "err");
				break;
		}
		printf("\n");
	}
	printf("\n");
}

// Prints out error and stops program.
void printlexerror(int type)
{
	if (type == 1)
	{
		printf("Lexical Analyzer Error: Invalid Identifier\n");
		exit(0);
	}
	else if (type == 2)
	{
		printf("Lexical Analyzer Error: Number Length\n");
		exit(0);
	}
	else if (type == 3)
	{
		printf("Lexical Analyzer Error: Identifier Length\n");
		exit(0);
	}
	else if (type == 4)
	{
		printf("Lexical Analyzer Error: Invalid Symbol\n");
		exit(0);
	}
	else
	{
		printf("Implementation Error: Unrecognized Error Type\n");
		exit(0);
	}
	free(list);
	return;
}
