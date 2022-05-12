%{
#include "y.tab.h"
void yyerror(char *);
%}
alpha [A-Za-z]
digit [0-9]

%%
"while" {return WHILE;}
"==" {return EQ;}
"="  {return ASIGN;}

[,{}();] {return *yytext;}
[ \t\n]+ ;

{digit}+    return NUM;
{alpha}({alpha}|{digit})*    return ID;

. {printf("\n\nlex err");}
%%
int yywrap()
{
return 1;
}