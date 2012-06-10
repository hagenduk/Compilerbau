#include "ircode.h"
#include "SymTab.h"
#include "diag.h"
#include <stdio.h>
#include <stdlib.h>


#define BACKP -2;
/*
 * For Throwing errors
 */
extern void yyerror (const char *msg);

/*
 * Counter for correct array (containing ir_codes) assign
 */
int ir_count = -1;

struct ir_struct *container=NULL;


void ir_entry(enum op_codes op, entry *var0, entry *var1, entry *var2, int jmp) {
	ir_count++;
	struct ir_struct *buffer = (struct ir_struct*) realloc (container, ir_count * sizeof(struct ir_struct));
	container = buffer;
	if (buffer == NULL) {
	FATAL_OS_ERROR(OUT_OF_MEMORY, 0, "buffer -> realloc");
	return;
	}

	if(var0==NULL) {
		var0 = newentry();
		var1->
	}

	container[ir_count].op = op;
	container[ir_count].var0 = var0;
	container[ir_count].var1 = var1;
	container[ir_count].var2 = var2;
	container[ir_count].jmp = op;
	container[ir_count].label = NULL;
}
/*
 * Array assignment
 * TODO ARRAY ASSIGN NOT GIVEN!
 */
void ir_entry_assign(entry *var0, entry *var1) {
	if (var0->type==2) {
		if (var0->position=NULL) {
			ir_entry(IR_ARR_ASSIGN, (struct entry *) var0->name,var0->position, var1->name , NULL, -1);
		}else if(var0->position!=NULL){

		}
	} else if(var0->type==1) {
		ir_entry(IR_ASSIGN, var0, var1, NULL, NULL);
	} else {
		//TODO More detailed Errors
		yyerror ("Assign Error!");
	}
}

}

