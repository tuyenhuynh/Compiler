%{
#include "a.tab.h"
#include <stdio.h>
%}

%%
([-]?)[0-9]*\.[0-9]*		{sscanf(yytext, "%f", &yylval.floatVal); return NUMBER;}
([-]?)[0-9]+				{sscanf(yytext, "%f", &yylval.floatVal); return NUMBER;}
"("					{return LEFT_P;}
")"					{return RIGHT_P;}
\n					{}
.					{}
%%
