/* 
 * parser.y - Parser utility for the DHBW compiler
 */
 
%{
	#include <stdio.h>
	#include "diag.h"
	#include "SymbTab.c"
	#include "ircode.c"
	
	//#define YYERROR_VERBOSE
	struct SymbTab *tablePtr;
	int numberOfParameters = 0;
	int isParam = 0;
	int functionType;
	int idForNumCounter = 0;
	int errorCounter = 0;
	int returnType=0;
	
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
//%type <entryStruct> function_parameter
%type <entryStruct> identifier_declaration
%type <entryStruct> function_declaration
%type <entryStruct> MARKER_FUNCTION_BEGIN
%type <entryStruct> expression
%type <entryStruct> primary

%%

program
     : { tablePtr=init_table(); } program_element_list	{ printf("\n\nTHERE ARE %d ERROR(S)!\n\n", errorCounter); }
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
													printf("%d> Wrong type declaration of >>%s<< as \"void\".\n", yylineno, $2->name);
													errorCounter++;
												}
											}
	;
	

identifier_declaration
	: ID BRACKET_OPEN NUM BRACKET_CLOSE		{ // Array Entry erstellen:
												if( !isParam ) {
													if( get_name(tablePtr, $1) ) {
														printf("%d> Array >>%s<< allready declared.\n", yylineno, $1);
														errorCounter++;
													} else {
														$$ = new_entry(tablePtr, $3, $1, 0, 2, 0);
													}
												} else {	//Parameter
													if(functionType == 5)  {//erstmalige function definition/declaration
														if(exists_param(tablePtr, $1)==NULL) {
															new_param(tablePtr, $1, 1);
															$$ = NULL;
														} else {
															printf("%d> Parameter >>%s<< allready exist.\n", yylineno, $1);
															errorCounter++;
														}
													} else if (functionType == 3) {
														if ( getTypeOfParam(tablePtr,numberOfParameters) == -1) {
															printf("%d> Parameter >>%s<<: Too many parameters? .\n", yylineno, $1);
															errorCounter++;
														} else if( getTypeOfParam(tablePtr,numberOfParameters) == 1) {
															printf("%d> Parameter >>%s<<: Wrong type of parameter? .\n", yylineno, $1);
															errorCounter++;
														}
													}
													isParam = 0;
												}
											}
	| ID									{ // INT Entry erstellen:
												if( !isParam ) {
													if( get_name(tablePtr, $1) ) {
														printf("%d> Variable >>%s<< allready declared.\n", yylineno, $1);
														errorCounter++;
													} else {
														$$ = new_entry(tablePtr, 1, $1, 0, 1, 0);
													}
												} else {	//Parameter
													if(functionType == 5)  {//erstmalige function definition/declaration
														if(exists_param(tablePtr, $1)==NULL) {
															new_param(tablePtr, $1, 1);
															$$ = NULL;
														} else {
															printf("%d> Parameter >>%s<< allready exist.\n", yylineno, $1);
															errorCounter++;
														}
													} else if (functionType == 3) {
														if ( getTypeOfParam(tablePtr,numberOfParameters) == -1) {
															printf("%d> Parameter >>%s<<: Too many parameters? .\n", yylineno, $1);
															errorCounter++;
														} else if( getTypeOfParam(tablePtr,numberOfParameters) == 2) {
															printf("%d> Parameter >>%s<<: Wrong type of parameter? .\n", yylineno, $1);
															errorCounter++;
														}
													}
													isParam = 0;
												}
											}
	;


function_definition
    : MARKER_FUNCTION_BEGIN PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE
						 {
							if( $1->type == 5 || $1->type == 3 ) {
								if(returnType==getReturnType(tablePtr)){ //returntypefehler
									if(getParamCnt(tablePtr)==0){
										tablePtr = end_function( tablePtr, numberOfParameters );
										$1->type = 4;
										}
										else{
											printf("%d> Function >>%s<< parameter mismatch.\n", yylineno, $1->name);
											errorCounter++;
										}
								}
								else{
									printf("%d> Function >>%s<< returntype mismatch.\n", yylineno, $1->name);
									errorCounter++;
									}
							} else {
								printf("%d> Function >>%s<< was allready declared.\n", yylineno, $1->name);
								errorCounter++;
							}
							numberOfParameters = 0;
						 }
    | MARKER_FUNCTION_BEGIN function_parameter_list PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE
						{
							if( $1->type == 5 || $1->type == 3 ) {
								if(returnType==getReturnType(tablePtr)){
									tablePtr = end_function( tablePtr, numberOfParameters );
									$1->type = 4;
								}
								else{
									printf("%d> Function >>%s<< returntype mismatch.\n", yylineno, $1->name);
									errorCounter++;
									}
							} else {
								printf("%d> Function >>%s<< was allready declared.\n", yylineno, $1->name);
								errorCounter++;
							}
							numberOfParameters = 0;
						}
	;


function_declaration
	: MARKER_FUNCTION_BEGIN PARA_CLOSE
						{
							if($1->type==5) {
								if(returnType==getReturnType(tablePtr)){
									if(getParamCnt(tablePtr)==0){
										tablePtr = end_function( tablePtr, numberOfParameters );
										$1->type = 3;
									}
										else{
											printf("%d> Function >>%s<< parameter mismatch.\n", yylineno, $1->name);
											errorCounter++;
										}
								}
								else{
									printf("%d> Function >>%s<< returntype mismatch.\n", yylineno, $1->name);
									errorCounter++;
									}
							} else if($1->type==3) {
								printf("%d> Function >>%s<< was allready defined.\n", yylineno, $1->name);
								errorCounter++;
							} else if($1->type==4) {
								printf("%d> Function >>%s<< was allready declared.\n", yylineno, $1->name);	
								errorCounter++;
							}
							numberOfParameters = 0;
						}

	| MARKER_FUNCTION_BEGIN function_parameter_list PARA_CLOSE
											{
												if($1->type==5) {
													if(returnType==getReturnType(tablePtr)){
														tablePtr = end_function( tablePtr, numberOfParameters );
														$1->type = 3;
													}
												else{
														printf("%d> Function >>%s<< returntype mismatch.\n", yylineno, $1->name);
														errorCounter++;
													}
												} else if($1->type==3) {
													printf("%d> Function >>%s<< was allready defined.\n", yylineno, $1->name);
													errorCounter++;
												} else if($1->type==4) {
													printf("%d> Function >>%s<< was allready declared.\n", yylineno, $1->name);	
													errorCounter++;
												}
												numberOfParameters = 0;
											}
	;

MARKER_FUNCTION_BEGIN
	: type ID PARA_OPEN	{ //TODO Wenn Prototyp dann überschreiben erlauben, Typ und Parametervergleich
							if( exists_entry(tablePtr,$2) && get_name(tablePtr, $2)->function != NULL ) {
								$$ = get_name(tablePtr, $2);
								tablePtr = $$->function;//get_function(get_rootptr(), $2);
							} else {
								tablePtr = decfunction( tablePtr, $2, 5, $1 );
								$$ = get_name( get_rootptr() , $2);
							}
							
							functionType = $$->type;
							returnType = getReturnType(tablePtr);
						}
	;

function_parameter_list
	: function_parameter
	| function_parameter_list COMMA function_parameter
	;
	
function_parameter
     : type { isParam = 1; }
		 identifier_declaration		{ //Entry von identifier_declaration weiterreichen
									//	$$ = $3;
										numberOfParameters++;
										if( 0 == $1 ) {
											printf("%d> Void as type of a parameter is not allowed.\n", yylineno);
											errorCounter++;
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
		 {
			 ir_return(IR_RETURN,$2);
			 if(getReturnType(tablePtr)==0){
			 	printf("%d> Void has no return statements.\n", yylineno);
				 errorCounter++;
			 }
			 else if( getReturnType(tablePtr) != $2->type ) {
				 printf("%d> Wrong return type.\n", yylineno);
				 errorCounter++;
			 }
		 }
     | RETURN SEMICOLON
		 {
			 if( getReturnType(tablePtr) != 0) {
				 printf("%d> Wrong return type.\n", yylineno);
				 errorCounter++;
    	 	}
		 }
     | SEMICOLON /* empty statement */
     ;

stmt_block
     : BRACE_OPEN stmt_list BRACE_CLOSE
     ;
	
stmt_conditional
     : IF PARA_OPEN expression {ir_if($3);ir_goto();} PARA_CLOSE stmt {backp_if(0);}
     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE {backp_if(1);ir_goto();} stmt {backp_if(0);}
     ;
									
stmt_loop
     : WHILE{ir_while_begin();} PARA_OPEN expression {ir_while($4);ir_while_goto_begin();} PARA_CLOSE stmt {backp_while();}
     | DO {ir_do_while_begin();} stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON { ir_do_while_end($6);}
     ;
									
expression
     : expression ASSIGN expression
			{
				 ir_assign($1, $3);
				 $$ = $3;
			}
     | expression LOGICAL_OR expression
			 {
				 $$ = ir_2exp(IR_LOGICAL_OR, $1, $3);
			 }
     | expression LOGICAL_AND expression
			 {
				 $$ = ir_2exp(IR_LOGICAL_AND, $1, $3); 
			 }
     | LOGICAL_NOT expression
			 {
				 $$ = ir_1exp(IR_LOGICAL_NOT, $2); 
			 }
     | expression EQ expression
			 {
				 $$ = ir_2exp(IR_EQ, $1, $3); 
			 }
     | expression NE expression
			 {
				 $$ = ir_2exp(IR_NE, $1, $3); 
			 }
     | expression LS expression
			 {
				 $$ = ir_2exp(IR_LS, $1, $3); 
			 } 
     | expression LSEQ expression
			 {
				 $$ = ir_2exp(IR_LSEQ, $1, $3); 
			 } 
     | expression GTEQ expression
			 {
				 $$ = ir_2exp(IR_GTEQ, $1, $3); 
			 } 
     | expression GT expression
			 {
				 $$ = ir_2exp(IR_GT, $1, $3); 
			 }
     | expression PLUS expression
			 {
				 $$ = ir_2exp(IR_PLUS, $1, $3); 
			 }
     | expression MINUS expression
			 {
				 $$ = ir_2exp(IR_MINUS, $1, $3); 
			 }
     | expression MUL expression
			 {
				 $$ = ir_2exp(IR_MUL, $1, $3); 
			 }
     | MINUS expression %prec UNARY_MINUS
			 {
				 $$ = ir_1exp(IR_MINUS, $2); 
			 }
     | ID BRACKET_OPEN primary BRACKET_CLOSE
     | PARA_OPEN expression PARA_CLOSE
			 {
				$$ = $2; 
			 }
     | function_call	
			 {//TODO $1 = return entry type
				 
			 }
     | primary
			 {
				$$ = $1; 
			 }
     ;

primary
     : NUM
		   {
			 $$ = new_entry(tablePtr, 1, NULL, 0, 1, 0);
			 sprintf($$->name,"NUM%d",idForNumCounter);
			 $$->value = $1;
			 
			 idForNumCounter++;
		   }
     | ID
		   {
			 struct param *p = exists_param(tablePtr, $1);
			 //struct param *p = NULL;
			 if( exists_entry(tablePtr, $1) ) {
				 $$ = get_name(tablePtr, $1);
			 } else if(p != NULL){
				 $$ = getParamAsEntry(p);
			 } else {
				 printf("%d> Primary >>%s<< was not declared.\n", yylineno, $1);
				 errorCounter++;
				 $$ = new_entry(tablePtr, 1, NULL, 0, 1, 0);
				 sprintf($$->name,"UNKNOWN%d",idForNumCounter);
				 $$->value = 1;	//default value of unknown variables
				 idForNumCounter++;
			 }
		   }
     ;

function_call
      : ID MARKER_BEGIN_FC PARA_OPEN PARA_CLOSE					{//
																	if( (get_function( tablePtr, $1))->paramCnt != numberOfParameters ) {
																		printf("%d> Too many parameters for function >>%s<<.\n", yylineno , $1);
																		errorCounter++;
																	}
																	numberOfParameters = 0;
																}
      | ID MARKER_BEGIN_FC PARA_OPEN function_call_parameters PARA_CLOSE		{//
																				  if( get_function( tablePtr, $1)->paramCnt != numberOfParameters ) {
																					  printf("%d> Number of parameters does not match to the declaration of function >>%s<<.\n", yylineno);
																					  errorCounter++;
																				  }
																				  numberOfParameters = 0;
																				}
      ;
      
MARKER_BEGIN_FC
	:															{// Epsilon TODO: $1 = ID
																//get_function(tablePtr, $1)
																}
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
