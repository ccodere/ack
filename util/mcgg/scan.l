%{
#include <stdbool.h>
#include "iburg.h"
#include "y.tab.h"

static int braces = 0;

%}
%option warn
%option nodefault
%option noyywrap
%option yylineno
%option debug

%x QSTRING
%x COMMENT

%%

<INITIAL>"\""               BEGIN(QSTRING);
<QSTRING>"\""               BEGIN(INITIAL);

<QSTRING>[%$][a-zA-Z_][a-zA_Z_0-9]+ {
                                yylval.string = strdup(yytext);
                                return QFRAGMENT;
                            }

<QSTRING>[^\r\n%$"]+        {
                                yylval.string = strdup(yytext);
                                return QFRAGMENT;
                            }

<INITIAL>"/*"               BEGIN(COMMENT);
<COMMENT>"*/"               BEGIN(INITIAL);
<COMMENT>[^*]*              ;
<COMMENT>"*"                ;

"DECLARATIONS"              return DECLARATIONS;
"PATTERNS"                  return PATTERNS;
"REGISTERS"                 return REGISTERS;
"corrupted"                 return CORRUPTED;
"cost"                      return COST;
"emit"                      return EMIT;
"fragment"                  return FRAGMENT;
"prefers"                   return PREFERS;
"when"                      return WHEN;
"with"                      return WITH;
"=="                        return EQUALS;
"!="                        return NOTEQUALS;

"//"[^\n]*\n                ;

[A-Za-z_][A-Za-z0-9_]*      { yylval.string = strdup(yytext); return ID; }
-?[0-9]+                    { yylval.n = atoi(yytext); return INT; }
-?0x[0-9a-fA-F]+            { yylval.n = strtol(yytext, NULL, 0); return INT; }
[ \t\r\n]*                  ;
.                           return yytext[0];

%%
/* vim: set sw=4 ts=4 expandtab : */