#include "mpc.h"

// if compiling on Windows, use these functions
#ifdef _WIN32

#include <string.h>

const int BUFFER_SIZE = 2048;
static char buffer[BUFFER_SIZE];

char* readline(char* prompt)
{
	fputs(prompt, stdout);
	fgets(buffer, BUFFER_SIZE, stdin);
	char* copy = malloc(strlen(buffer) + 1);
	strcpy(copy, buffer);
	copy[strlen(copy) - 1] = '\0';
	
	return copy;
}

// dummy function
void add_history(char* unused) { }

// otherwise include editline header 
#else
#include <editline/readline.h>
#endif

int main(int argc, char** argv) 
{
	// create parsers
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expression = mpc_new("expression");
	mpc_parser_t* Lisp = mpc_new("lisp");
	
	// define parsers with the following language
	mpca_lang(MPCA_LANG_DEFAULT,
        "                                                               \
            number      : /-?[0-9]+/ ;                                  \
            operator    : '+' | '-' | '*' | '/' | '%' |                 \
                         \"add\" | \"sub\" | \"mul\" | \"div\" ;        \
            expression  : <number> | '(' <operator> <expression>+ ')' ; \
            lisp        : /^/ <operator> <expression>+ /$/ ;            \
        ",
		Number, Operator, Expression, Lisp);

	
	// print version and exit information 
	puts("Lisp Version 0.0.0.2");
	puts("Press Ctrl+c to exit\n");
	
	while (1)
	{
		// output prompt and get input
		char* input = readline("b_lisp> ");
		
		// add input to history
		add_history(input);
		
		// attempt to parse user input
		mpc_result_t result;

		if (mpc_parse("<stdin>", input, Lisp, &result)) 
		{
			// success - print the AST
			mpc_ast_print(result.output);
			mpc_ast_delete(result.output);
		}
		else
		{
			// failure - print error
			mpc_err_print(result.error);
			mpc_err_delete(result.error);
		}
		
		/*
		// echo input back to user
		printf("Echoing: %s\n", input);
		*/
		
		free(input);
	}	
	
	// undefine and delete parsers
	mpc_cleanup(4, Number, Operator, Expression, Lisp);
	
	return  0;
}