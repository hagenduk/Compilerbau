/* 
 * parser.y - Parser utility for the DHBW compiler
 */
 
%{
	#include <stdio.h>
	#include "diag.h"
	#include "SymbTab.c"
	#include "ircode.c"
	#include "stack.c"
	
	//#define YYERROR_VERBOSE
	struct SymbTab *tablePtr;
	int numberOfParameters = 0;
	int numberOfParametersCall = 0;
	int isParam = 0;
	int functionType;
	int idForNumCounter = 0;
//	int errorCounter = 0;
	int returnType=0;
	struct entry *fcPointer;
	struct SymbTab *savePtr;
	
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
%type <entryStruct> function_call
%type <num> type
//%type <entryStruct> function_parameter
%type <entryStruct> identifier_declaration
%type <entryStruct> function_declaration
%type <entryStruct> MARKER_FUNCTION_BEGIN
%type <entryStruct> MARKER_BEGIN_FC
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
													if( exists_entry(tablePtr, $1) ) {
														printf("%d> Array >>%s<< allready declared.\n", yylineno, $1);
														errorCounter++;
													} else {
														if( get_name(tablePtr, $1) == NULL )
															$$ = new_entry(tablePtr, $3, $1, 0, 2, 0);
													}
												} else {	//Parameter
													if(functionType == 5)  {//erstmalige function definition/declaration
														if(exists_param(tablePtr, $1)==NULL) {
															new_param(tablePtr, $1, 2);
															$$ = NULL;
														} else {
															printf("%d> Parameter >>%s<< allready exist.\n", yylineno, $1);
															errorCounter++;
														}
													} else if (functionType == 3) {
														if ( getTypeOfParam(tablePtr,numberOfParameters) == -1) {
															printf("%d> Parameter >>%s<<: Too many parameters?\n", yylineno, $1);
															errorCounter++;
														} else if( getTypeOfParam(tablePtr,numberOfParameters) == 1) {
															printf("%d> Parameter >>%s<<: Wrong type of parameter?\n", yylineno, $1);
															errorCounter++;
														}
													}
													isParam = 0;
												}
											}
	| ID									{ // INT Entry erstellen:
												if( !isParam ) {
													if( exists_entry(tablePtr, $1) ) {
														printf("%d> Variable >>%s<< allready declared.\n", yylineno, $1);
														errorCounter++;
													} else {
														if( get_name(tablePtr, $1) == NULL)
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
															printf("%d> Parameter >>%s<<: Too many parameters?\n", yylineno, $1);
															errorCounter++;
														} else if( getTypeOfParam(tablePtr,numberOfParameters) == 2) {
															printf("%d> Parameter >>%s<<: Wrong type of parameter?\n", yylineno, $1);
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
    	printf("START function_definition");
							if( $1->type == 5 ) {
								$1->type = 4;
								tablePtr = end_function( tablePtr, numberOfParameters );
							} else if( $1->type == 3 ) {
								if(returnType==getReturnType(tablePtr)){ //returntypefehler
									printf("Parameter Count: %d", getParamCnt(tablePtr));
									if(getParamCnt(tablePtr)==0){
										tablePtr = end_function( tablePtr, numberOfParameters );
										$1->type = 4;
									} else {
										printf("%d> -B- Function >>%s<< parameter mismatch.\n", yylineno, $1->name);
										errorCounter++;
									}
								} else {
									printf("%d> Function >>%s<< returntype mismatch.\n", yylineno, $1->name);
									errorCounter++;
								}
							} else {
								printf("%d> Function  >>%s<< was allready declared (or name is used).\n", yylineno, $1->name);
								errorCounter++;
							}
//							ir_func_end($1);
							numberOfParameters = 0;
			printf("END function_definition");
						 }
    | MARKER_FUNCTION_BEGIN function_parameter_list PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE
						{
    	printf("START function_definition");
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

//							ir_func_end($1);
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
											printf("%d> -A- Function >>%s<< parameter mismatch.\n", yylineno, $1->name);
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

//							ir_func_end($1);
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

//												ir_func_end($1);
												numberOfParameters = 0;
											}
	;

MARKER_FUNCTION_BEGIN
	: type ID PARA_OPEN	{ //TODO Wenn Prototyp dann überschreiben erlauben, Typ und Parametervergleich
		printf("START MARKER_FUNCTION_BEGIN");
							if( exists_entry(tablePtr,$2) ) {
								if(get_name(tablePtr, $2)->function != NULL) {
									$$ = get_name(tablePtr, $2);
									tablePtr = $$->function;//get_function(get_rootptr(), $2);
								} else {
								tablePtr = decfunction( tablePtr, $2, 5, $1 );
								$$ = get_name( get_rootptr() , $2);
								}
							} else {
								tablePtr = decfunction( tablePtr, $2, 5, $1 );
								$$ = get_name( get_rootptr() , $2);
							}
							
							functionType = $$->type;
							returnType = getReturnType(tablePtr);
							printf("\nMITTE MARKER_FUNCTION_BEGIN %d\n", returnType);
							ir_func_begin($$);
							printf("END MARKER_FUNCTION_BEGIN\n");
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
     | {printf("GEH IN expression\n");} expression {printf("GEH AUS expression\n");} SEMICOLON
     | stmt_conditional
     | stmt_loop
     | RETURN expression SEMICOLON
		 {
			 ir_return(IR_RETURN,$2);
			 
			if(getReturnType(tablePtr)==0){
			 	printf("%d> Void has no return statements.\n", yylineno);
				 errorCounter++;
			 }
			 else if( $2->function != NULL) {
				 if(getReturnType(tablePtr) != getReturnType($2->function)){
				 printf("%d> Wrong return type(fc).\n", yylineno);
				 errorCounter++;}

			 }
			 else{
				 if(getReturnType(tablePtr) != $2->type){
				 printf("%d> Wrong return type(n).\n", yylineno);
				 printf("expected %d type was %d",getReturnType(tablePtr), $2->type);
				 errorCounter++;}			 	
			 }
		 }
     | RETURN SEMICOLON
		 {
			 ir_return(IR_RETURN,NULL);
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
//     : IF PARA_OPEN expression {ir_if($3);ir_goto();} PARA_CLOSE stmt {backp_if(0);}
//     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE {backp_if(1);ir_goto();} stmt {backp_if(0);}
	 : IF PARA_OPEN expression {ir_if($3);ir_goto();} PARA_CLOSE MARKER_STMT_CONDITIONAL
     ;

MARKER_STMT_CONDITIONAL
	 : stmt {backp_if(0);}
     | stmt ELSE {backp_if(1);ir_goto();} stmt {backp_if(0);}
     ;
									
stmt_loop
     : WHILE{ir_while_begin();} PARA_OPEN expression PARA_CLOSE {ir_while($4);ir_while_goto_begin();} stmt {backp_while();}
     | DO {ir_do_while_begin();} stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON { ir_do_while_end($6);}
     ;
									
expression
     : expression ASSIGN expression
			{
    	 printf("START ASSIGN\n");
				 ir_assign($1, $3);
				 $$ = $3;
				 if(checktype($1->type,$3->type)==0){
				 	 printf("%d> type mismatch or cannot calc with void.\n", yylineno);
					 errorCounter++;
    	 		}
    	 		 	if(checkexpr($1)==0){
    	 			 		    	printf("%d> cannot calc with void.\n", yylineno);
									errorCounter++;
									}
					if(checkexpr($3)==0){
    	 			 		    	printf("%d> cannot calc with void.\n", yylineno);
									errorCounter++;
					}
		printf("END ASSIGN\n");
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
			 	printf("%d Plus %d", $1->type, $3->type);
				 $$ = ir_2exp(IR_PLUS, $1, $3);
				 	if(checktype($1->type,$3->type)==0 && $3->type!=1){
				 	 	printf("%d> type mismatch or cannot calc with void.\n", yylineno);
					 	errorCounter++;
    	 			}
			 }
     | expression MINUS expression
			 {
				 $$ = ir_2exp(IR_MINUS, $1, $3);
				 if(checktype($1->type,$3->type)==0 && $3->type!=1){
				 	 	printf("%d> type mismatch or cannot calc with void.\n", yylineno);
					 	errorCounter++;
    	 			} 
			 }
     | expression MUL expression
			 {
				 $$ = ir_2exp(IR_MUL, $1, $3);
				 if(checktype($1->type,$3->type)==0 && $3->type!=1){
				 	 	printf("%d> type mismatch or cannot calc with void.\n", yylineno);
					 	errorCounter++;
    	 			}

			 }
     | MINUS expression %prec UNARY_MINUS
			 {
				 $$ = ir_1exp(IR_MINUS, $2);
				 if($2->type!=1){
				 	 	printf("%d> type mismatch.\n", yylineno);
					 	errorCounter++;
    	 			}
    	 			 if($2->function!=NULL){
					 	if(getReturnType($2->function)==0){
					 		printf("%d> cannot calc with void.\n", yylineno);
					 		errorCounter++;
					 	}
					 }  
			 }
     | ID BRACKET_OPEN primary BRACKET_CLOSE
			{
			   struct param *p = exists_param(tablePtr, $1);
			   struct entry *e;
			   //if(exists_entry(tablePtr, $1) ) {
			   if(get_name(tablePtr, $1) != NULL) {
					e = get_name(tablePtr,$1);
					e->position = $3;
					$$ = ir_assign_arr(e, $3);
				} else if(p != NULL){
					e = getParamAsEntry(tablePtr,p);
					e->position = $3;
					$$ = ir_assign_arr(e, $3);
				} else {
					printf("%d> Unknown array >>%s<<.\n", yylineno, $1);
				}
			}
     | PARA_OPEN expression PARA_CLOSE
			 {
				$$ = $2; 
			 }
     | function_call	
			 { 
			 //printf("expression ergebnis %d ",getReturnType($1->function));
			 
			 $$=$1;
				 
			 }
     | primary
			 {
				$$ = $1; 
			 }
     | expression SHIFT_LEFT expression
     | expression SHIFT_RIGHT expression
     ;

primary
     : NUM
		   {
			 printf("START NUM");
		  	 char *c;
			 c=malloc(20);
			 sprintf(c, "%d", $1);
		   	
		   	 if( get_name(tablePtr, c) != NULL ) {
		   	 	$$ = get_name(tablePtr, c);
		   	 } else {
		   	 	$$ = new_entry(tablePtr, 1, c, 0, 1, 0);
				$$->value = $1;
		   	 }
		   }
     | ID
		   {
    	printf("START ID");
		   if(savePtr==NULL) printf("DIE POINTER");
			 struct param *p =exists_param(tablePtr,$1); // exists_param(savePtr,$1); Problem, savePtr wird erst später in 581 gesetzt....
			 //struct param *p = NULL;
			 //if( exists_entry(tablePtr, $1) ) {
			 if( get_name(tablePtr, $1) != NULL ) {	//<- beachtet auch globale Variablen, deswegen reicht exists_entry nicht aus
				 $$ = get_name(tablePtr, $1);
				 printf("da");
			 } else if(p != NULL){
				printf("bla");
				 $$ = getParamAsEntry(tablePtr,p);
			 } else {
				 printf("%d> Primary >>%s<< was not declared.\n", yylineno, $1);
				 errorCounter++;
				 char *c;
				 c=malloc(20);
				 sprintf(c,"UNK%d",idForNumCounter);
				 $$ = new_entry(tablePtr, 1, c, 0, 1, 0);
				 $$->value = 1;	//default value of unknown variables
				 idForNumCounter++;
			 }
		printf("END ID\n");
		   }
     ;

function_call
      : MARKER_BEGIN_FC PARA_CLOSE					{
																	//if( (getParamCnt(get_function( tablePtr, $1))) != numberOfParameters ) {
																	//	printf("%d> Too many parameters for function >>%s<<.\n", yylineno , $1);
																	//	errorCounter++;
																	//}
																	if(getFirstParam(fcPointer->function) != NULL) {
																					printf("%d> Function needs parameters.\n", yylineno);
																					errorCounter++;
																		}
																	$$=fcPointer;
																	//printf("fc ergebnis %d ", getReturnType(fcPointer->function));
																	numberOfParametersCall = 0;
																	savePtr = stack_pop();
																}
      | MARKER_BEGIN_FC function_call_parameters PARA_CLOSE		{
																				 // if( get_function( tablePtr, $1)->paramCnt != numberOfParameters ) {
																				//	  printf("%d> Number of parameters does not match to the declaration of function >>%s<<.\n", yylineno);
																				//	  errorCounter++;
																				//  }
																				  
																				 
																				  $$=fcPointer;
																				  numberOfParametersCall = 0;
																				  savePtr = stack_pop();
																				}
      ;
      
MARKER_BEGIN_FC
	:	ID PARA_OPEN							{
																
																numberOfParametersCall = 0;
																fcPointer=get_name(get_rootptr(), $1);
																if(savePtr!= NULL){
																	stack_push(savePtr);
																	savePtr=fcPointer->function;
																}else {
																	stack_init();
																	savePtr=fcPointer->function;
																}
																int tmpType = 0;
																if( get_name(get_rootptr(), $1) != NULL) {
																			struct entry *e = get_name(get_rootptr(), $1);
																			if(e->type != 4 && e->type != 3) {
																				printf("%d> -A-Function >>%s<< was not defined. -%d-\n", yylineno , $1, e->type);
																				errorCounter++;
																			} else {
																				tmpType = e->function->returntype;
																			}
																		printf("hi");
																		$$ = ir_funccall(e, ir_find_FuncDef(e) );
																		$$->type = tmpType;
																	}
												}
	;      

function_call_parameters
     : function_call_parameters COMMA expression
     				{
     					if($3->type!=getParamType(savePtr, numberOfParametersCall)){
     						printf("%d> Parameter is of wrong type or missing(m).\n", yylineno);
				 			printf("Wert 1 ist: %d Wert 2 ist: %d",$3->type,getParamType(savePtr, numberOfParametersCall));
				 			errorCounter++;
     					}
     					numberOfParametersCall++;
     				}
     			
     | expression {		
     					if($1->type!=getParamType(savePtr, numberOfParametersCall)){
     						printf("%d> -A-Parameter is of wrong type or missing(s).\n", yylineno);
     						printf("\nTYPE P=%d F=%d C=%d\n\n", $1->type, getParamType(savePtr, numberOfParametersCall), numberOfParametersCall);
				 			printf("Wert 1 ist: %d Wert 2 ist: %d",$1->type,getParamType(savePtr, numberOfParametersCall));
				 			errorCounter++;
     					}
     					numberOfParametersCall++;
     				}
     ;

%%

void yyerror (const char *msg)
{
  printf("ERROR: %s\n", msg);
}
