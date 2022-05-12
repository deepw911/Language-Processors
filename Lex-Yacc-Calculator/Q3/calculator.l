
%{
/* Definition section */
#include<stdio.h>

void yyerror(char *);
#include "y.tab.h"
%}


%%
[0-9]+(\.[0-9]+)?  {
		yylval.dtype=atof(yytext);
		return NUMBER;

	}
[\t] ;

[\n] return 0;

. return yytext[0];

%%

int yywrap()
{
return 1;
}