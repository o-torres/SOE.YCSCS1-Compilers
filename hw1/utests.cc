//
// See copyright.h for copyright notice and limitation of liability
// and disclaimer of warranty provisions.
//
#include "copyright.h"

//////////////////////////////////////////////////////////////////////////////
//
//  lextest.cc
//
//  Reads input from file argument.
//
//  Option -l prints summary of flex actions.
//
//////////////////////////////////////////////////////////////////////////////

#include <assert.h>      // needed on Linux system
#include <stdio.h>      // needed on Linux system
#include <unistd.h>     // for getopt
#include "cool-parse.h" // bison-generated file; defines tokens
#include "utilities.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <string>

//
//  The lexer keeps this global variable up to date with the line number
//  of the current line read from the input.
//
int curr_lineno = 1;
char *curr_filename = "<stdin>"; // this name is arbitrary
FILE *fin;   // This is the file pointer from which the lexer reads its input.

//
//  cool_yylex() is the function produced by flex. It returns the next
//  token each time it is called.
//
extern int cool_yylex();
YYSTYPE cool_yylval;           // Not compiled with parser, so must define this.

extern int optind;  // used for option processing (man 3 getopt for more info)

//
//  Option -v sets the lex_verbose flag. The main() function prints out tokens
//  if the program is invoked with option -v.  Option -l sets yy_flex_debug.
//
extern int yy_flex_debug;      // Flex debugging; see flex documentation.
extern int lex_verbose;        // Controls printing of tokens.
void handle_flags(int argc, char *argv[]);

//
//  The full Cool compiler contains several debugging flags, all of which
//  are handled and set by the routine handle_flags.  Here we declare
//  cool_yydebug, which is not used by the lexer but is needed to link
//  with handle_flags.
//
int  cool_yydebug;

// defined in utilities.cc
extern void dump_cool_token(ostream& out, int lineno, 
			    int token, YYSTYPE yylval);

extern void yyset_debug(int);
extern char *cool_token_to_string(int);

int fails = 0;
using namespace std;

void do_test(std::vector<char*> txt, std::vector<int> &tokens, std::vector<char*> msg) {
    fin = fopen("utest.cl", "w");
    for (unsigned int i = 0; i < txt.size(); i++) {
        fputs(txt[i], fin);
    }
    fflush(fin);
    fclose(fin);
    fin = fopen("utest.cl", "r");
    curr_lineno = 1;
    vector<char*>::iterator msg_iter = msg.begin();
    for (unsigned int i = 0; i < tokens.size(); i++) {
        int token = cool_yylex();
        if (token != tokens[i]) {
            cout << "FAIL: expected: " << cool_token_to_string(tokens[i]) << \
                    ", got: " << cool_token_to_string(token) << endl;
            fails++;
        }
        else {
            char * token_name = cool_token_to_string(token);
            switch(token) {
                case BOOL_CONST:
                    if (msg_iter == msg.end()) {
                        cout << "FAIL: no message for BOOL" << endl;
                        fails++;
                    }
                    else {
                         int exp_int = atoi(*msg_iter);
                         if (exp_int != cool_yylval.boolean) {
                             cout << "FAIL: expected BOOL: " << exp_int << \
                                     ", got: " << cool_yylval.boolean << endl;
                             fails++;
                         }
                         msg_iter++;
                    }
                    break;
                case TYPEID: 
                case OBJECTID: 
                case INT_CONST: 
                case STR_CONST: 
                    if (msg_iter == msg.end()) {
                        cout << "FAIL: no message for " <<  token_name << endl;
                        fails++;
                    }
                    else {
                         if (cool_yylval.symbol->equal_string(*msg_iter, strlen(*msg_iter)) == 0) {
                             cout << "FAIL: expected " << token_name << ": " << *msg_iter << \
                                     ", got: " << cool_yylval.symbol->get_string() << endl;
                             fails++;
                         }
                         msg_iter++;
                    }
                    break;
                case ERROR: 
                    if (msg_iter == msg.end()) {
                        cout << "FAIL: no message for " <<  token_name << endl;
                        fails++;
                    }
                    else {
                         if (strcmp(*msg_iter, cool_yylval.error_msg) != 0) {
                             cout << "FAIL: expected " << token_name << ": " << *msg_iter << \
                                     ", got: " << cool_yylval.error_msg << endl;
                             fails++;
                         }
                         msg_iter++;
                    }
                    break;
                 default:
                    break;
            }
        }
    }
    fclose(fin);

}

int main(int argc, char** argv) {
    yyset_debug(1);
    std::vector<int> tokens;
    std::vector<char*> str;
    std::vector<char*> msg;

    str.push_back("+/-*=<.~,;:()@{}\n");
    tokens.push_back('+');
    tokens.push_back('/');
    tokens.push_back('-');
    tokens.push_back('*');
    tokens.push_back('=');
    tokens.push_back('<');
    tokens.push_back('.');
    tokens.push_back('~');
    tokens.push_back(',');
    tokens.push_back(';');
    tokens.push_back(':');
    tokens.push_back('(');
    tokens.push_back(')');
    tokens.push_back('@');
    tokens.push_back('{');
    tokens.push_back('}');

    str.push_back("class\n");
    tokens.push_back(CLASS);

    str.push_back("else\n");
    tokens.push_back(ELSE);

    str.push_back("if\n");
    tokens.push_back(IF);

    str.push_back("fi\n");
    tokens.push_back(FI);

    str.push_back("in\n");
    tokens.push_back(IN);

    str.push_back("inherits\n");
    tokens.push_back(INHERITS);

    str.push_back("let\n");
    tokens.push_back(LET);

    str.push_back("loop\n");
    tokens.push_back(LOOP);

    str.push_back("pool\n");
    tokens.push_back(POOL);

    str.push_back("then\n");
    tokens.push_back(THEN);

    str.push_back("while\n");
    tokens.push_back(WHILE);

    str.push_back("case\n");
    tokens.push_back(CASE);

    str.push_back("esac\n");
    tokens.push_back(ESAC);

    str.push_back("of\n");
    tokens.push_back(OF);

    str.push_back("=>\n");
    tokens.push_back(DARROW);

    str.push_back("new\n");
    tokens.push_back(NEW);

    str.push_back("isvoid\n");
    tokens.push_back(ISVOID);

    str.push_back("<-\n");
    tokens.push_back(ASSIGN);

    str.push_back("not\n");
    tokens.push_back(NOT);

    str.push_back("<=\n");
    tokens.push_back(LE);

    str.push_back("id_123\n");
    msg.push_back("id_123");
    tokens.push_back(OBJECTID);

    str.push_back("Id_123\n");
    msg.push_back("Id_123");
    tokens.push_back(TYPEID);

    str.push_back("true\n");
    msg.push_back("1");
    tokens.push_back(BOOL_CONST);

    str.push_back("tRUE\n");
    msg.push_back("1");
    tokens.push_back(BOOL_CONST);

    str.push_back("tRuE\n");
    msg.push_back("1");
    tokens.push_back(BOOL_CONST);

    str.push_back("false\n");
    msg.push_back("0");
    tokens.push_back(BOOL_CONST);

    str.push_back("fAlsE\n");
    msg.push_back("0");
    tokens.push_back(BOOL_CONST);

    str.push_back("1234567890\n");
    msg.push_back("1234567890");
    tokens.push_back(INT_CONST);

    str.push_back("\"1234567890\"\n");
    msg.push_back("1234567890");
    tokens.push_back(STR_CONST);

    str.push_back("\"a\" a A 1 -- a comment\n");
    tokens.push_back(STR_CONST);
    msg.push_back("a");
    tokens.push_back(OBJECTID);
    msg.push_back("a");
    tokens.push_back(TYPEID);
    msg.push_back("A");
    tokens.push_back(INT_CONST);
    msg.push_back("1");

    str.push_back("-- this is a comment!\n");
    str.push_back("(* this is also a comment! *)\n");
    str.push_back("(* this is a\n");
    str.push_back("* multi-line\n");
    str.push_back("* comment\n");
    str.push_back("*)");

    str.push_back("\n\f\t \n");

    str.push_back("\"This is a multi-line\\\n");
    str.push_back("string constant\"");
    tokens.push_back(STR_CONST);
    msg.push_back("This is a multi-line\nstring constant");

    do_test(str, tokens, msg);
    str.clear();
    tokens.clear();
    msg.clear();

    str.push_back("(*"); // unfinished comment is an error
    msg.push_back("EOF in comment");
    tokens.push_back(ERROR);
    do_test(str, tokens, msg);
    str.clear();
    tokens.clear();
    msg.clear();

    str.push_back("\""); // unfinished string is an error
    msg.push_back("EOF in string constant");
    tokens.push_back(ERROR);
    do_test(str, tokens, msg);
    str.clear();
    tokens.clear();
    msg.clear();

    str.push_back("*)"); // unfinished string is an error
    msg.push_back("unmatched *)");
    tokens.push_back(ERROR);
    do_test(str, tokens, msg);
    str.clear();
    tokens.clear();
    msg.clear();

    str.push_back("\"l1\\\n\"\n");
    str.push_back("\"l2\n");
    str.push_back("\"l3\t\"\n");
    str.push_back("\"l4\n");
    str.push_back("\"l5\"");
    msg.push_back("l1\n");
    tokens.push_back(STR_CONST);
    msg.push_back("Unterminated string constant");
    tokens.push_back(ERROR);
    msg.push_back("l3\t");
    tokens.push_back(STR_CONST);
    msg.push_back("Unterminated string constant");
    tokens.push_back(ERROR);
    msg.push_back("l5");
    tokens.push_back(STR_CONST);
    do_test(str, tokens, msg);
    str.clear();
    tokens.clear();
    msg.clear();

    //char null_arr[3] = {'"', 0, '"'};
    //str.push_back(null_arr);
    //msg.push_back("String contains NULL character");
    //tokens.push_back(ERROR);
    //do_test(str, tokens, msg);
    //str.clear();
    //tokens.clear();
    //msg.clear();

    if (fails == 0) {
        cout << "All Tests Passed" << endl;
    }
    else {
        cout << "Total Fails: "<< fails << endl;
    }
    exit(fails);
}



