/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST+1]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int nested_comments;

int string_buf_add(unsigned int len, char * str);

%}

/*
 * Define names for regular expressions here.
 */

CLASS      (?i:class)
ELSE       (?i:else)
FI         (?i:fi)
IF         (?i:if)
IN         (?i:in)
INHERITS   (?i:inherits)
LET        (?i:let)
LOOP       (?i:loop)
POOL       (?i:pool)
THEN       (?i:then)
WHILE      (?i:while)
CASE       (?i:case)
ESAC       (?i:esac)
OF         (?i:of)
DARROW     =>
NEW        (?i:new)
ISVOID     (?i:isvoid)
STR_CONST  \"[^\"]*\"
INT_CONST  [0-9]+
TYPEID     [A-Z][A-Za-z0-9_]*
OBJECTID   [a-z][A-Za-z0-9_]*
ASSIGN     <-
NOT        (?i:not)
LE         <=
BOOL_TRUE  t(?i:rue)
BOOL_FALSE f(?i:alse)

%Start IN_COMMENT IN_STRING STRING_ERROR

%%


<IN_COMMENT>{
"(*"         { nested_comments++; }
"*)"         { nested_comments--;
               if(nested_comments == 0) {
                   BEGIN(INITIAL);
               }
             }
[^*\n\(]+    // eat comment in chunks
"*"          // eat the lone star
"("          // eat the lone star
\n           curr_lineno++;
<<EOF>>      { BEGIN(INITIAL);
               cool_yylval.error_msg = "EOF in comment";
               return ERROR;
             }
}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

<IN_STRING>{
\"           { BEGIN(INITIAL);
               *string_buf_ptr = 0;
               cool_yylval.symbol = stringtable.add_string(string_buf);
               return STR_CONST;
             }
(\\t)|\t     { if (string_buf_add(1, "\t")) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
(\\b)|\b     { if (string_buf_add(1, "\b")) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
(\\f)|\f     { if (string_buf_add(1, "\f")) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
(\\\n)|(\\n) { curr_lineno++;
               if (string_buf_add(1, "\n")) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
\\\000       { BEGIN(STRING_ERROR);
               cool_yylval.error_msg = "String contains escaped null character.";
               return ERROR;
             }
\000         { BEGIN(STRING_ERROR);
               cool_yylval.error_msg = "String contains null character.";
               return ERROR;
             }
\\.          { if (string_buf_add(1, &yytext[1])) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
[^\\\"\n\0]+ { if (string_buf_add(yyleng, yytext)) {
                   BEGIN(STRING_ERROR);
                   cool_yylval.error_msg = "String constant too long";
                   return ERROR;
               }
             }
\n           { BEGIN(INITIAL);
               curr_lineno++;
               cool_yylval.error_msg = "Unterminated string constant";
               return ERROR;
             }

<<EOF>>      { BEGIN(INITIAL);
               cool_yylval.error_msg = "EOF in string constant";
               return ERROR;
             }
}
<STRING_ERROR>{
\"           { BEGIN(INITIAL); }
\\\n         { curr_lineno++; }
\\.          ;
\n           { BEGIN(INITIAL);
               curr_lineno++;
             }
[^\\\"\n]+     ;
<<EOF>>      { BEGIN(INITIAL);
             }
}
<INITIAL>{
"(*"         {nested_comments = 1;
              BEGIN(IN_COMMENT);
             }
\"           {string_buf_ptr = string_buf;
              BEGIN(IN_STRING);
             }

"*)"         { cool_yylval.error_msg = "unmatched *)";
               return ERROR;
             }
 
{CLASS}      { return CLASS; }
{ELSE}       { return ELSE; }
{FI}         { return FI; }
{IF}         { return IF; }
{IN}         { return IN; }
{INHERITS}   { return INHERITS; }
{LET}        { return LET; }
{LOOP}       { return LOOP; }
{POOL}       { return POOL; }
{THEN}       { return THEN; }
{WHILE}      { return WHILE; }
{CASE}       { return CASE; }
{ESAC}       { return ESAC; }
{OF}         { return OF; }
{DARROW}     { return DARROW; }
{NEW}        { return NEW; }
{ISVOID}     { return ISVOID; }
{ASSIGN}     { return ASSIGN; }
{NOT}        { return NOT; }
{LE}         { return LE; }
"+"          { return '+'; }
"/"          { return '/'; }
"-"          { return '-'; }
"*"          { return '*'; }
"="          { return '='; }
"<"          { return '<'; }
"."          { return '.'; }
"~"          { return '~'; }
","          { return ','; }
";"          { return ';'; }
":"          { return ':'; }
"("          { return '('; }
")"          { return ')'; }
"@"          { return '@'; }
"{"          { return '{'; }
"}"          { return '}'; }
{BOOL_TRUE}  { cool_yylval.boolean = 1;
               return BOOL_CONST;
             }
{BOOL_FALSE} { cool_yylval.boolean = 0;
               return BOOL_CONST;
             }
{TYPEID}     { cool_yylval.symbol = idtable.add_string(yytext);
               return TYPEID;
             }
{OBJECTID}   { cool_yylval.symbol = idtable.add_string(yytext);
               return OBJECTID;
             }
{INT_CONST}  { cool_yylval.symbol = inttable.add_string(yytext);
               return INT_CONST;
             }

--.*         ;

\n           curr_lineno++;
[ \t\f\b\r\v] ;
.            { cool_yylval.error_msg = yytext;
               return ERROR;
             }

}
%%

unsigned int string_buf_size() {
    return string_buf_ptr - string_buf;
}

int string_buf_add(unsigned int len, char * str) {
    unsigned int new_sz = string_buf_size() + len;
    if (new_sz >= MAX_STR_CONST) {
        return 1;
    }
    while(len--) {
        *string_buf_ptr++ = *str++;
    }
    return 0;
}
