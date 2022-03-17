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

// Global variable checking for valid symbols. 
int isValid = 1;

int alphatoken();
int numbertoken();
int symboltoken();
int comment();
int reservedcheck(char *buffer);
void printlexerror(int type);
void printtokens();


// Helper function to get identifiers, numbers, and reserved words from input string.
void input_processor(char* input)
{
	// Reserved words
	if (strcmp(input, "var") == 0)
	{
		list[lex_index].type = varsym;
		lex_index++;
	}
	else if (strcmp(input, "procedure") == 0)
	{
		list[lex_index].type = procsym;
		lex_index++;
	}
	else if (strcmp(input, "call") == 0)
	{
		list[lex_index].type = callsym;
		lex_index++;
	}
	else if (strcmp(input, "begin") == 0)
	{
		list[lex_index].type = beginsym;
		lex_index++;
	}
	else if (strcmp(input, "end") == 0)
	{
		list[lex_index].type = endsym;
		lex_index++;
	}
	else if (strcmp(input, "if") == 0)
	{
		list[lex_index].type = ifsym;
		lex_index++;
	}
	else if (strcmp(input, "do") == 0)
	{
		list[lex_index].type = dosym;
		lex_index++;
	}
	else if (strcmp(input, "while") == 0)
	{
		list[lex_index].type = whilesym;
		lex_index++;
	}
	else if (strcmp(input, "read") == 0)
	{
		list[lex_index].type = readsym;
		lex_index++;
	}
	else if (strcmp(input, "write") == 0)
	{
		list[lex_index].type = writesym;
		lex_index++;
	} // Special symbols
	else if (input[0] == '.')
	{
		list[lex_index].type = periodsym;
		lex_index++;
	}
	else if (input[0] == '[')
	{
		list[lex_index].type = lbracketsym;
		lex_index++;
	}
	else if (input[0] == ']')
	{
		list[lex_index].type = rbracketsym;
		lex_index++;
	}
	else if (input[0] == ',')
	{
		list[lex_index].type = commasym;
		lex_index++;
	}
	else if (input[0] == ';')
	{
		list[lex_index].type = semicolonsym;
		lex_index++;
	}
	else if (input[0] == '?')
	{
		list[lex_index].type = questionsym;
		lex_index++;
	}
	else if (input[0] == '(')
	{
		list[lex_index].type = lparenthesissym;
		lex_index++;
	}
	else if (input[0] == ')')
	{
		list[lex_index].type = rparenthesissym;
		lex_index++;
	}
	else if (input[0] == '+')
	{
		list[lex_index].type = addsym;
		lex_index++;
	}
	else if (input[0] == '-')
	{
		list[lex_index].type = subsym;
		lex_index++;
	}
	else if (input[0] == '*')
	{
		list[lex_index].type = multsym;
		lex_index++;
	}
	else if (input[0] == '/')
	{
		list[lex_index].type = divsym;
		lex_index++;
	}
	else if (input[0] == '%')
	{
		list[lex_index].type = modsym;
		lex_index++;
	} // Identifiers
	else if (isalpha(input[0]) != 0)
	{
		list[lex_index].type = identsym;
		lex_index++;
	} // Numbers
	else if (isdigit(input[0]) != 0)
	{
		list[lex_index].type = numbersym;
		lex_index++;
	} // If the input doesn't match print symbol error.
	else
	{
		printlexerror(4);
		isValid = 0;
	}

}


lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;
	int inputIndex = 0;
	int currentPos = 0;
	int tempNum[MAX_NUMBER_LEN];

	// Go through given input string until EOF
	while (input[inputIndex] != '\0')
	{
		// Set for symbol error check
		isValid = 1;

		// Check for whitespace and space character.
		if (iscntrl(input[inputIndex]) != 0 || isspace(input[inputIndex]) != 0)
		{
			inputIndex++;
			continue;
		} // Check for starting letter
		else if (isalpha(input[inputIndex] != 0))
		{
			// Position after checking for whitespace.
			currentPos = inputIndex;

			// Token begins with letter so read characters until you reach one that is not alphanumeric.
			for (; inputIndex < currentPos + MAX_IDENT_LEN; inputIndex++)
			{
				// Check for Identifier Length error.
				if (inputIndex - currentPos >= MAX_IDENT_LEN)
				{
					printlexerror(3);
					return NULL;
				}
				else if (isdigit(input[inputIndex]) != 0)
				{
					continue;
				}
				else if (isalpha(input[inputIndex]) == 0)
				{
					strncpy(list[lex_index].name, &input[currentPos], inputIndex - currentPos);
					input_processor(list[lex_index].name);

					// Check for invalid symbol.
					if (!isValid)
					{
						return NULL;
					}
				}
			}
		} // Check for starting number
		else if (isdigit(input[inputIndex]) != 0)
		{
			// Position after checking for whitespace.
			currentPos = inputIndex;

			// Token begins with number so read digits until you reach one that is not a number.
			for (; inputIndex <= currentPos + MAX_NUMBER_LEN; inputIndex++)
			{
				// Check for Number Length Error
				if (inputIndex - currentPos > MAX_NUMBER_LEN)
				{
					printlexerror(2);
					return NULL;
				} // Check for Invalid Identifier
				else if (isalpha(input[inputIndex]) != 0)
				{
					printlexerror(1);
					return NULL;
				}
				else if (isdigit(input[inputIndex]) == 0)
				{
					strncpy(list[lex_index].name, &input[currentPos], inputIndex - currentPos);
					break;
				}
			}

			list[lex_index].value = atoi(list[lex_index].name);
			list[lex_index].type = numbersym;
			lex_index++;
		} // Check for starting / for comment.
		else if (input[inputIndex] == '/')
		{
			currentPos = inputIndex;

			// Check for subsequent / for comment.
			if (input[inputIndex + 1] == '/')
			{
				// Comments end on new line OR EOF.
				while (input[inputIndex] != '\n' || input[inputIndex] != '\0')
				{
					inputIndex++;
				}
			}
		} // Check for all possible special symbols starting with < .
		else if (input[inputIndex] == '<')
		{
			// Not equal
			if (input[inputIndex + 1] == '>')
			{
				list[lex_index].type = neqsym;
				lex_index++;
				inputIndex += 2;
			} // Less than or equal to
			else if (input[inputIndex + 1] == '=')
			{
				list[lex_index].type = leqsym;
				lex_index++;
				inputIndex += 2;
			} // Less than
			else
			{
				list[lex_index].type = lsssym;
				lex_index++;
				inputIndex++;
			}
		} // Check for all possible special symbols starting with > .
		else if (input[inputIndex] == '>')
		{
			// Greater than or equal to
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index].type = geqsym;
				lex_index++;
				inputIndex += 2;
			} // Greater than
			else
			{
				list[lex_index].type = gtrsym;
				lex_index++;
				inputIndex++;
			}
		} // Check for all possible special symbols starting with : .
		else if (input[inputIndex] == ':')
		{
			// Assignment
			if (input[inputIndex + 1] == '=')
			{
				list[lex_index].type = assignsym;
				lex_index++;
				inputIndex += 2;
			} // Colon
			else
			{
				list[lex_index].type = colonsym;
				lex_index++;
				inputIndex++;
			}
		} // Equal to
		else if (input[inputIndex] == '=')
		{
			if (input[inputIndex] == '=')
			{
				list[lex_index].type = eqlsym;
				lex_index++;
				inputIndex += 2;
			} // Check for invalid symbol.
			else
			{
				printlexerror(4);
				return NULL;
			}
		}
		else
		{
			input_processor(&input[inputIndex]);
			inputIndex++;

			// Check for invalid symbol.
			if (!isValid)
			{
				return NULL;
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