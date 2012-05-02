/* 
 * parser.y - Parser utility for the DHBW compiler
 */
 
%{
	#include <stdio.h>
	#include "SymbTab.c"
	
	//#define YYERROR_VERBOSE
	struct SymbTab *tablePtr;
	int numberOfParameters = 0;
	
	extern int    yylineno;
%}

%union{
	int num;
	char* id;

	struct SymbTab *tableStruct;
	struct entry *entryStruct;
};
 
%debug
%locations
%start program

/*
 * One shift/reduce conflict is expected for the "dangling-else" problem. This
 * conflict however is solved by the default behavior of bison for shift/reduce 
 * conflicts (shift action). The default behavior of bison corresponds to what
 * we want bison to do: SHIFT if the lookahead is 'ELSE' in order to bind the 'ELSE' to
 * the last open if-clause. 
 */
%expect 1

%token DO WHILE
%token IF ELSE
%token <id>INT <id>VOID
%token RETURN
%token COLON COMMA SEMICOLON
%token BRACE_OPEN BRACE_CLOSE

%token <id>ID
%token <num>NUM

%right ASSIGN 
%left  LOGICAL_OR
%left  LOGICAL_AND
%left  EQ NE     
%left  LS LSEQ GTEQ GT 
%left  SHIFT_LEFT SHIFT_RIGHT
%left  PLUS MINUS     
%left  MUL
%right LOGICAL_NOT UNARY_MINUS UNARY_PLUS
%left  BRACKET_OPEN BRACKET_CLOSE PARA_OPEN PARA_CLOSE

//%type <> function_call_parameters
//%type <> function_definition
//%type <entryStruct> function_declaration
//%type <> function_call
%type <num> type
%type <entryStruct> function_parameter
%type <entryStruct> identifier_declaration
//%type <> expression
//%type <> primary

%%

program
     : { tablePtr=init_table(); } program_element_list
     ;

program_element_list
     : program_element_list program_element
     | program_element
     ;

program_element
     : variable_declaration SEMICOLON
     | function_declaration SEMICOLON
     | function_definition
     | SEMICOLON
     ;
									

type
	: INT		{ $$ = 1; }
	| VOID		{ $$ = 0; }
	;
     

variable_declaration
	: variable_declaration COMMA identifier_declaration
	| type identifier_declaration			{ 
												if( 0 == $1 ) {
													// TODO yyerror für die Fehlerausgabe verwenden.
													printf("> Wrong type declaration of >>%s<< as \"void\" at line %d\n", $2->name, yylineno);
													//yyerror("Wrong type declaration");// of \"" + $2->name + "\"");
												}
											}
	;
	

identifier_declaration
	: ID BRACKET_OPEN NUM BRACKET_CLOSE		{ // Array Entry erstellen:
												// TODO Pruefen, ob Variable in diesem Scope bereits deklariert wurde.
												$$ = new_entry(tablePtr, $3, $1, 0, 2, 0);
											}
	| ID									{ // INT Entry erstellen:
												// TODO Pruefen, ob Variable in diesem Scope bereits deklariert wurde.
												$$ = new_entry(tablePtr, 1, $1, 0, 1, 0);
											}
	;


function_definition
    : MARKER_FUNCTION_BEGIN PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE								{
																										tablePtr = end_function( tablePtr );
																										numberOfParameters = 0;
																									}
    | MARKER_FUNCTION_BEGIN function_parameter_list PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE		{
																										tablePtr = end_function( tablePtr );
																										numberOfParameters = 0;
																									}
	;


function_declaration
	: MARKER_FUNCTION_BEGIN PARA_CLOSE								{ tablePtr = end_function( tablePtr ); numberOfParameters = 0; }
	| MARKER_FUNCTION_BEGIN function_parameter_list PARA_CLOSE		{ tablePtr = end_function( tablePtr ); numberOfParameters = 0; }
	;

MARKER_FUNCTION_BEGIN
	: type ID PARA_OPEN	{ //
							// TODO Pruefen, ob Funktion schon deklariert wurde
							if(is_root_table(tablePtr)==1) printf("hallo");
							tablePtr = decfunction( tablePtr, $2 , $1 );
						}
	;


function_parameter_list
	: function_parameter
	| function_parameter_list COMMA function_parameter
	;
	
function_parameter
     : type identifier_declaration		{ // Entry von identifier_declaration weiterreichen
											 $2->scope=1;
											 $$ = $2;
											 numberOfParameters++;
											 if( 0 == $1 ) {
												 // TODO yyerror für die Fehlerausgabe verwenden.
												 printf("> Wrong type declaration of >>%s<< as \"void\" at line %d\n", $2->name, yylineno);
												 //yyerror("Wrong type declaration");// of \"" + $2->name + "\"");
											}
     									}
     ;
									
stmt_list
     : /* empty: epsilon */
     | stmt_list stmt
     ;

stmt
     : stmt_block
     | variable_declaration SEMICOLON
     | expression SEMICOLON
     | stmt_conditional
     | stmt_loop
     | RETURN expression SEMICOLON
     | RETURN SEMICOLON
     | SEMICOLON /* empty statement */
     ;

stmt_block
     : BRACE_OPEN stmt_list BRACE_CLOSE
     ;
	
stmt_conditional
     : IF PARA_OPEN expression PARA_CLOSE stmt
     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE stmt
     ;
									
stmt_loop
     : WHILE PARA_OPEN expression PARA_CLOSE stmt
     | DO stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON
     ;
									
expression
     : expression ASSIGN expression
     | expression LOGICAL_OR expression
     | expression LOGICAL_AND expression
     | LOGICAL_NOT expression
     | expression EQ expression
     | expression NE expression
     | expression LS expression 
     | expression LSEQ expression 
     | expression GTEQ expression 
     | expression GT expression
     | expression PLUS expression
     | expression MINUS expression
     | expression MUL expression
     | MINUS expression %prec UNARY_MINUS
     | ID BRACKET_OPEN primary BRACKET_CLOSE
     | PARA_OPEN expression PARA_CLOSE
     | function_call
     | primary
     ;

primary
     : NUM
     | ID
     ;

function_call
      : ID PARA_OPEN PARA_CLOSE
      | ID PARA_OPEN function_call_parameters PARA_CLOSE
      ;

function_call_parameters
     : function_call_parameters COMMA expression
     | expression
     ;

%%

void yyerror (const char *msg)
{
  printf("ERROR: %s\n", msg);
}