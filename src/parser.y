/* 
 * parser.y - Parser utility for the DHBW compiler
 */
 
%{
	#include <stdio.h>
	#include "SymbTab.c"
	
	//#define YYERROR_VERBOSE

	struct SymbTab *tablePtr;
	struct entry *entryPtr;
%}

%union{
	char* id;
	int num;
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
%token <id>INT <num>VOID
%token RETURN
%token COLON COMMA SEMICOLON
%token BRACE_OPEN BRACE_CLOSE

%token ID
%token NUM

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

%%

program
     : { tablePtr = (struct SymTap *) malloc (sizeof (struct SymTab));
     	 entryPtr = (struct entry *) malloc (sizeof (struct entry)); } program_element_list { printf("\tprogram\n")}
     ;

program_element_list
     : program_element_list program_element	//{ printf("\tprogram_element_list\n")}
     | program_element 						//{ printf("\tprogram_element_list\n")}
     ;

program_element
     : variable_declaration SEMICOLON	//{ printf("\tprogram_element\n"); }
     | function_declaration SEMICOLON
     | function_definition
     | SEMICOLON
     ;
									
type
     : INT		{ printf("\tType: INT\n")  }
     | VOID		{ printf("\tType: VOID\n") }
     ;

variable_declaration
     : variable_declaration COMMA identifier_declaration
     | type identifier_declaration			//{ printf("\tvariable_declaration\n"); }
     ;

identifier_declaration
     : ID BRACKET_OPEN NUM BRACKET_CLOSE	{	printf("\tArray: MIST[%d]\n", yylval.num);
     											new_entry(tablePtr,yylval.num,"MIST",0,2,0);
     											//new_entry(tablePtr,$3,$1,0,2,0);
     										}
     | ID 									{	printf("\tID: %s\n", yylval.id);
     											entryPtr = get_name(tablePtr, yylval.id);
     											if (entryPtr == NULL) {
     												new_entry(tablePtr,1,yylval.id,0,1,0);
     												//new_entry(tablePtr,1,$1,0,1,0);
     											} else {
     												yyerror("Variable wurde bereits deklariert!");
     											}
     										}
     ;

function_definition
     : type ID PARA_OPEN PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE							{ printf("\tfunction_definition\n"); }
     | type ID PARA_OPEN function_parameter_list PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE	{ printf("\tfunction_definition\n"); }
     ;

function_declaration
     : type ID PARA_OPEN PARA_CLOSE								{	printf("\tfunction_declaration\n");
     																//new_entry(tablePtr,5,yylval.id,0,4,0); 
     															}
     | type ID PARA_OPEN function_parameter_list PARA_CLOSE		{ printf("\tfunction_declaration\n"); }
     ;

function_parameter_list
     : function_parameter
     | function_parameter_list COMMA function_parameter
     ;
	
function_parameter
     : type identifier_declaration								{	printf("\tfunction_parameter\n");
     				 												
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