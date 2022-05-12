%{
#include<stdio.h>
#include<string.h>
int i = 0;
%}


%%

	/* strings having certain words */
^[a-zA-Z ]*deep|walke|vnit[a-zA-Z ]*$ {printf("string having either 'deep', 'walke','vnit'");} 
	
	/* strings having fifth element as 'a' */           
^[a-zA-Z]{4}a[a-zA-Z ]*$ {printf("%s having fifth element as 'a'\n",yytext);}

	/* strings having last letter as 'p' */
^.*p$ {printf("string having last letter as 'p'\n");}

	/* strings having exactly 2 letters as 'ab' */
^ab$ {printf("string having exactly 2 letters as 'ab'\n");}

	/* strings having one Capital, one special and one numerical */

^.*[0-9].*[@#$%^&-+=\(\)].*[A-Z].*$  {printf("one caps one special one numerical\n");}
^.*[0-9].*[A-Z].*[@#$%^&-+=\(\)].*$  {printf("one caps one special one numerical\n");}
^.*[A-Z].*[@#$%^&-+=\(\)].*[0-9].*$  {printf("one caps one special one numerical\n");}
^.*[A-Z].*[0-9].*[@#$%^&-+=\(\)].*$  {printf("one caps one special one numerical\n");}
^.*[@#$%^&-+=\(\)].*[0-9].*[A-Z].*$  {printf("one caps one special one numerical\n");}
^.*[@#$%^&-+=\(\)].*[A-Z].*[0-9].*$  {printf("one caps one special one numerical\n");}

^(0?[1-9]|1[0-2]):[0-5][0-9]$ {printf("strings have time\n");}
.+

%%
  
int yywrap(void){}
  
int main()
{   
    // The function that starts the analysis
    yylex();
  
    return 0;
}

