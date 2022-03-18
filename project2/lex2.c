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

// Keeps position within input.
int inputIndex = 0;

// Keeps track of active errors.
int errorCheck = 0;

int alphatoken(char *input);
int numbertoken(char *input);
int symboltoken(char *input);
void comment(char *input, int inputIndex);
int reservedcheck(char *buffer);
void printlexerror(int type);
void printtokens();

// Check for word.
int alphatoken(char *input)
{
    //NOT SURE TO INITALIZE TEMP ARRAY TO 1000...
    char temp[MAX_NUMBER_TOKENS];
    int tempindex = 0;
    while(input[inputIndex] != '\0')
    {
        //if current character is a comment 	
	if(input[inputIndex] == '/' && input[inputIndex +1] == '/')
	{
		comment(input, inputIndex);
	}
        //if current character its a letter
        else if(isalpha(input[inputIndex]) != 0)
        {
            temp[tempindex++] = input[inputIndex++];
        }
	//if current character it is any type of whitespace replace it with single space 
        else if(iscntrl(input[inputIndex]) != 0 || isspace(input[inputIndex]) != 0)
        {
            temp[tempindex++] = ' ';
        }
    }
    //split up the words as individual strings
    char *token = strtok(words , " \t\n\r");
    while( token != NULL ) {
      reservedcheck(token);
 //Check if identifier length is greater than 11; returns 3 if identifier is out of range.
      if(strlen(token) > MAX_IDENT_LEN)
      {
          return 3;
      }
      token = strtok(NULL, " ");
    }
}

// Check for number.
int numbertoken(char *input)
{
	char temp[MAX_NUMBER_LEN + 1];

	int tempIndex = 0;

	// While input is a number copy over to temporary array.
	while(isdigit(input[inputIndex]) != 0 && tempIndex <= MAX_NUMBER_LEN)
	{
		temp[tempIndex++] = input[inputIndex++];
	}
	
	// If we break from previous while loop, we need to check why we broke out to print the correct error message 
	// Check if our numCount == 6 and if next currCharcter is a digit then its a NUMBER LENGTH ERROR
	if (tempIndex > MAX_NUMBER_LEN)
	{
		return 2;
	}
	
	// Check if next current Character is a letter then its a INVALID IDENTIFIER ERROR
	if(isalpha(input[inputIndex]) != 0)
	{
		return 1;
	}

	// Copy temp over to list and update list type.
	list[lex_index].type = numbersym;
	list[lex_index].value = atoi(temp);
	lex_index++;

	return 0;
}

// Check for special symbols.
int symboltoken(char *input)
{
	switch(input[inputIndex])
	{	
		case '.':
			list[lex_index++].type = periodsym;
			break;
		case '[': 
			list[lex_index++].type = lbracketsym;
			break;
		case ']':
			list[lex_index++].type = rbracketsym;
			break;
		case ',':
			list[lex_index++].type = commasym;
			break;
		case ';':
			list[lex_index++].type = semicolonsym;
			break;
		case ':':
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = assignsym;
				inputIndex += 2;
				return 0;
			}

			list[lex_index++].type = colonsym;
			break;
		case '?':
			list[lex_index++].type = questionsym;
			break;
		case '(':
			list[lex_index++].type = lparenthesissym;
			break;
		case ')':
			list[lex_index++].type = rparenthesissym;
			break;
		case '=':
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = eqlsym;
				inputIndex += 2;
				return 0;
			}
			// '=' is not a valid symbol (4. Invalid Symbol).
			return 4;
		case '<':
			if (input[inputIndex + 1] == '>')
			{
				list[lex_index++].type = neqsym;
				inputIndex += 2;
				return 0;
			}
			
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = leqsym;
				inputIndex += 2;
				return 0;
			}
			
			list[lex_index++].type = lsssym;
			break;
		case '>':
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index++].type = geqsym;
				inputIndex += 2;
				return 0;
			}
			
			list[lex_index++].type = gtrsym;
			break;
		case '+':
			list[lex_index++].type = addsym;
			break;
		case '-':
			list[lex_index++].type = subsym;
			break;
		case '*':
			list[lex_index++].type = multsym;
			break;
		case '/':
			// Check for comment.
			if (input[inputIndex + 1] == '/')
			{
				return 0;
			}
			// Otherwise it's a divide symbol.
			list[lex_index++].type = divsym;
			break;
		case '%':
			list[lex_index++].type = modsym;
			break;
		// If none of the symbols match it is an invalid symbol (4. Invalid Symbol).
		default:
			return 4;		
	}

	inputIndex++;
	return 0;
}

// Check for comment.
void comment(char *input, int inputIndex)
{
	//move the inputIndex until the comment is finished 
	while(input[inputIndex] != '\n' || input[inputIndex] != '\r\n')
      	{
		inputIndex++;
	}
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
        list[lex_index++].type = identsym;
   }
	return 0;
}


lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;
	

	// Loop to go through the input file as long as we're not at EOF and there are no errors.
	while(input[inputIndex] != '\0' && errorCheck == 0)
	{

		// Check for whitespace and space character.
		if (iscntrl(input[inputIndex]) != 0 || isspace(input[inputIndex]) != 0)
		{
			inputIndex++;
			continue;
		}
	
		// Check if current Character in the input is a number.
		else if(isdigit(input[inputIndex]))
		{
			errorCheck = numbertoken(input);

			// Handle case for (2. Number Length) or (1. Invalid Identifier)
			if (errorCheck)
			{
				printlexerror(errorCheck);
				return NULL;
			}	
		}
		
		// Check if current Character in the input is a character.
		else if(isalpha(input[inputIndex]))
		{
			errorCheck = alphatoken(input);

			// Handle case for (3. Identifier Length)
			if (errorCheck)
			{
				printlexerror(errorCheck);
				return NULL;
			}
		}
		else 
		{
			errorCheck = symboltoken(input);	
			
			// Handle case for (4. Invalid Symbol)
			if (errorCheck)
			{
				printlexerror(errorCheck);
				return NULL;
			}
			
		}
	}

	if (printFlag)
		printtokens();
	list[lex_index++].type = -1;
	
	// Return NULL if an error is encountered otherwise return the list.
	return errorCheck ? NULL : list;			

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

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}
