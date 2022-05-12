%{
#include<stdio.h>
void yyerror(char *);
int yylex();
%}


%start whileloop
%token WHILE EQ ASIGN ID NUM
%%
whileloop	: WHILE '(' cond ')' body {printf("parsing successful\n");} ;

cond	: VARIABLE EQ VARIABLE ;

VARIABLE	: ID 
			| NUM ;

body	: '{' statement '}' 
		| statement ;

statement 	: ID ASIGN VARIABLE ';' 
			| whileloop ;
	
%%

void yyerror(char *s)
{
	printf("\nparsing err\n");
}

int main()
{
	yyparse();
	return 0;
}