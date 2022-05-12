
%{
/* Definition section */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int yylex(void);
void yyerror(char *s); 

int flag=0;
%}



%union {double dtype;}

%token <dtype> NUMBER

%type <dtype> E ArithmeticExpression

%left '+' '-'

%left '*' '/' '%' 

%right '^'

%left '(' ')'

%%

ArithmeticExpression: E{

		printf("\nResult=%lf\n", $$);

		return 0;

		};
E:E'+'E {$$=$1+$3;}

|E'-'E {$$=$1-$3;}

|E'*'E {$$=$1*$3;}

|E'/'E {if($3==0)yyerror("Division by 0"),exit(0); else $$=$1/$3;}

|E'%'E {$$=($1*$3)/100;}

|E'^'E {
		if($1==0 && $3<=0)yyerror("Negative or zero exponent not allowed for base value zero"),exit(0);
		else if(1.0*((int)$3) != $3 && $1<0)yyerror("Exponent cannot be fraction value if base is negative"),exit(0) ;
		else $$=pow($1,$3);
	}

|'('E')' {$$=$2;}

| '-' NUMBER {$$=-$2;}

| NUMBER {$$=$1;}

;

%%

//driver code
void main()
{
printf("\nEnter Any Arithmetic Expression \n");

yyparse();

}

void yyerror(char *s)
{
printf("\n Invalid Expression: %s \n",s);
flag=1;
}


