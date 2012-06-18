/**
 * @brief 	constructed with the help of
 * 			http://foja.dcs.fmph.uniba.sk/kompilatory/docs/compiler.pdf
 * 			and Philipp Götze
 */
#include "ircode.h"
#include "SymbTab.h"
#include "diag.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * For Throwing errors
 */
extern void yyerror(const char *msg);

/*
 * Counter for correct array (containing ir_codes) assign
 */
int ir_count = 0;

/**
 * Counter for temp Variables
 */
int ir_tmp_counter = 0;

/**
 * Counter for Backpatch
 */
int jmp_count = 0;

/**
 * File the IR-Code will be written to
 */
FILE *ir_file;
/**
 * Number of entries, used to iterate array
 */
int code_count = 0;

/**
 * char buffer
 */
char s[200];

int errorCounter = 0;

struct ir_struct *container = NULL;
/**
 * Interace function to fill container containing the IR-Code
 */
void ir_entry(enum op_codes op, entry *var0, entry *var1, entry *var2, int jmp) {

	struct ir_struct *tmp = (struct ir_struct*) realloc(container, ir_count
			* sizeof(struct ir_struct));
	container = tmp;
	if (tmp == NULL) {
		FATAL_OS_ERROR(OUT_OF_MEMORY, 0, "tmp -> realloc");
		errorCounter++;
		return;
	}
	ir_count++;
	container[ir_count].op = op;
	container[ir_count].var0 = var0;
	container[ir_count].var1 = var1;
	container[ir_count].var2 = var2;
	container[ir_count].jmp = jmp;
}

/*
 * Assignment
 *
 */
void ir_assign(struct entry *var0, struct entry *var1) {
	if (var0->name == NULL) {
		yyerror("You cannot assign to an integer;");
	}
	if (var0->type == 0 || var1->type == 0) {
		yyerror("Cannot assign Void!");
	} else {
		if (var0->type == 2) {
			if (var0->position != NULL) {
				ir_entry(IR_ARR_ASSIGN, var0, var1, NULL, NULL);
			} else if (var0->position == NULL) {
				yyerror("Array without indexing cannot be assigned a value!");
			}
		} else if (var0->type == 1) {
			ir_entry(IR_ASSIGN, var0, var1, NULL, NULL);
		} else {
			yyerror("Assign Error!");
		}
	}
}

/**
 * For one expression (logical not,...)
 */
struct entry *ir_1exp(enum op_codes op, struct entry *var0) {
	struct entry *v = ir_tmp();
	if (var0->type == 0) {
		yyerror("Operands can not be of type VOID");
	}
	ir_entry(op, v, var0, NULL, NULL);
	printf("TEST 5");
	return v;
}

/**
 * Used for most op codes, like Plus, Mul,...
 */
struct entry *ir_2exp(enum op_codes op, struct entry *var1, struct entry *var2) {
	if (var1->type == 0 || var2->type == 0) {
		yyerror("Operands can not be of type VOID");
	}
	struct entry *v = ir_tmp();

	if (var1->type == 2 && var1->position == NULL) {
		yyerror("First Operand must have an array index");
	}
	if (var2->type == 2 && var2->position == NULL) {
		yyerror("Second Operand must have an array index");
	}
	ir_entry(op, v, var1, var2, NULL);
	return v;
}

/**
 * Return variable
 */
void ir_return(enum op_codes op, struct entry *var0) {
	if (op == IR_RETURN) {
		ir_entry(op, var0, NULL, NULL, -2);
	} else {
		yyerror("Fatal error!");
	}
}

/**
 * Assign array value to new temp
 */
struct entry *ir_assign_arr(struct entry *var0, struct entry *var1) {
	struct entry *v = ir_tmp();
	//TODO Hey, ich hab unten die Anweisung auskommentiert, weil die nen "Kreis" in der Symbol-Tabelle herstellt.
	//Das hat zu ner Endlosschleife gefuehrt.... musst mal schauen, ob du das brauchst oder iwie anders machen kannst.
	//v->next = var0;
	var0->position = var1->value;
	ir_entry(IR_ASSIGN_ARR, v, var0, NULL, NULL);
	return v;
}

/**
 * Label func begin
 */
void ir_func_begin(struct entry *var0){
ir_entry(IR_FUNC_START, var0, NULL, NULL, NULL);
}

/**
 * Label func end
 */
void ir_func_end(struct entry *var0){
ir_entry(IR_FUNC_END, var0, NULL, NULL, NULL);
}

/**
 * Label if
 */
void ir_if(struct entry *var0) {
	ir_entry(IR_IF, var0, NULL, NULL, ir_count + 2);
}

/**
 * Label Goto
 */
void ir_goto() {
	ir_entry(IR_GOTO, NULL, NULL, NULL, -2);
}

/**
 * Set While (Same as If)
 */
void ir_while(struct entry *var0) {
	ir_entry(IR_IF, var0, NULL, NULL, ir_count + 2);
}

/**
 * Label While Begin
 */
void ir_while_begin() {
	ir_entry(IR_WHILE_BEGIN, NULL, NULL, NULL, -2);
}

/**
 * Label While() goto begin
 */
void ir_while_goto_begin() {
	ir_entry(IR_GOTO, NULL, NULL, NULL, -2);
}

/**
 * Label while() do begin
 */
void ir_do_while_begin() {
	ir_entry(IR_DO_WHILE_BEGIN, NULL, NULL, NULL, -2);
}
/**
 * Set If for do while end and backpatch
 */
void ir_do_while_end(struct entry *var0) {
	struct ir_struct *cptr;
	int i;
	for (i = ir_count - 1; i >= 0; i--) {
		cptr = &container[i];
		if (cptr->op == IR_DO_WHILE_BEGIN) {
			if (cptr->jmp == -2) {
				cptr->jmp = -1;
				break;
			}
		}
	}
	ir_entry(IR_IF, var0, NULL, NULL, i);
}

/**
 * Set a Funccall
 */
struct entry *ir_funccall(struct entry *var0, int jmp) {
	struct entry *v = ir_tmp();
	ir_entry(IR_CALL, var0, v, NULL, jmp);
	return v;
}
/****************************************************BACKP*********************************************************/
void backp_if(int new) {
	struct ir_struct *c;
	for (int i = code_count - 1; i >= 0; i--) {
		c = &container[i];
		if (c->op == IR_GOTO) {
			if (c->jmp == -2) {
				c->jmp = ir_count + new;
				break;
			}
		}
	}
}

void backp_return() {
	struct ir_struct *c;
	for (int i = 0; i < ir_count; i++) {
		c = &container[i];
		if (c->op == IR_RETURN) {
			if (c->jmp == -2) {
				c->jmp = ir_count - 1;
			}
		}
	}
}

void backp_while() {
	struct ir_struct *c;
	for (int i = ir_count - 1; i >= 0; i--) {
		c = &container[i];
		if (c->op == IR_GOTO) {
			if (c->jmp == -2) {
				c->jmp = ir_count + 1;
				break;
			}
		} else if (c->op == IR_WHILE_BEGIN) {
			if (c->jmp == -2) {
				ir_entry(IR_GOTO, NULL, NULL, NULL, i);
				c->jmp = -1;
				break;
			}
		}
	}
}
/**********************************************************************************************************/
/**
 * Used to find the Function and set the jmp to there
 */
int ir_find_FuncDef(struct entry *var0) {
	struct ir_struct *c;
	for (int i = 0; i < ir_count; i++) {
		c = &container[i];
		if (c->var0->type == 4) {
			return i;
		}
	}
	return 0;
}
/**
 * Returns a Temp variable
 */
struct entry *ir_tmp() {
	root = get_rootptr();
	char *buffer[5];
	sprintf(buffer, ".tmp%d", ir_tmp_counter);
	ir_tmp_counter++;
	return new_entry(root, 1, buffer, 0, 1, 0);
}

/**
 * Used for Backpatching and Code Iteration
 */
int set_jmpLabel(int pos, int label) {
	if (container[pos].label == NULL) {
		if ((pos > 0) && (pos < ir_count)) {
			container[pos].label = label;
			return 1;
		}
	}
	return 0;
}

/**
 * Has to be called first
 */
void ir_set_file(FILE *file) {
	ir_file = file;
}

/**
 * Generate the actual IR-Code and write to file
 */
void generate_ir_code() {
	if (ir_file != NULL) {
		printf("IR START \n");
		struct ir_struct *c;
		struct param *param = NULL;
		char tab = '\0';
		sprintf(s, "--------- Intermediate Code ---------\n");
		fprintf(ir_file,"%s",s);
		sprintf(s,"%d",ir_count);
		fputs(s, ir_file);
		printf("start");
		for (int i = 0; i <= ir_count; i++) {
			c = &container[i];
			if (c->op == IR_FUNC_END || c->op == IR_PARA || c->op
					== IR_WHILE_BEGIN || c->op == IR_DO_WHILE_BEGIN) {
				tab = '\0';
			}
			if (c->label != NULL) {
				sprintf(s, ".l%d:\n", c->label);
				fputs(s, ir_file);
			}
			sprintf(s, "%c", tab);
			fputs(s, ir_file);
			switch (c->op) {
			case IR_ASSIGN:
				if (c->var1->name == NULL) {
					sprintf(s, "%s = %s", c->var0->name, c->var1->value);
					fputs(s, ir_file);
					break;
				} else {
					sprintf(s, "%s = %s", c->var0->name, c->var1->name);
					fputs(s, ir_file);
					break;
				}
			case IR_PLUS:
				sprintf(s, "%s = %s + %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_MINUS:
				sprintf(s, "%s = %s - %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_MUL:
				sprintf(s, "%s = %s * %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_UNARY_MINUS:
				sprintf(s, "%s = -%s", c->var0->name, c->var1->name);
				fputs(s, ir_file);
				break;
			case IR_SHIFT_LEFT:
				sprintf(s, "%s = %s << %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_SHIFT_RIGHT:
				sprintf(s, "%s = %s >> %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_LOGICAL_AND:
				sprintf(s, "%s = %s && %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_LOGICAL_OR:
				sprintf(s, "%s = %s || %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_LOGICAL_NOT:
				sprintf(s, "%s = !%s", c->var0->name, c->var1->name);
				fputs(s, ir_file);
				break;
			case IR_NE:
				sprintf(s, "%s = %s != %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_EQ:
				sprintf(s, "%s = %s == %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_GT:
				sprintf(s, "%s = %s > %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_GTEQ:
				sprintf(s, "%s = %s >= %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_LS:
				sprintf(s, "%s = %s < %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_LSEQ:
				sprintf(s, "%s = %s <= %s", c->var0->name, c->var1->name,
						c->var2->name);
				fputs(s, ir_file);
				break;
			case IR_IF:
				jmp_count++;
				if (!set_jmpLabel(c->jmp, jmp_count)) {
					jmp_count--;
					c->jmp = container[c->jmp].label;
				} else {
					c->jmp = jmp_count;
				}
				sprintf(s, "IF %s GOTO .l%d", c->var0->name, c->jmp);
				fputs(s, ir_file);
				break;
			case IR_GOTO:
				jmp_count++;
				if (!set_jmpLabel(c->jmp, jmp_count)) {
					jmp_count--;
					c->jmp = container[c->jmp].jmp;
				} else {
					c->jmp = jmp_count;
				}
				sprintf(s, "GOTO .l%d", c->jmp);
				fputs(s, ir_file);
				break;
			case IR_WHILE_BEGIN:
				jmp_count++;
				c->label = jmp_count;
				sprintf(s, ".l%d:\n", c->label);
				fputs(s, ir_file);
				break;
			case IR_DO_WHILE_BEGIN:
				jmp_count++;
				c->label = jmp_count;
				sprintf(s, ".l%d:\n", c->label);
				fputs(s, ir_file);
				break;
			case IR_RETURN:
				if (c->var0 != NULL)
					sprintf(s, "RETURN %s", c->var0->name);
				else
					sprintf(s, "RETURN");
				fputs(s, ir_file);
				break;
			case IR_PARA:
				tab = '\t';
				break;
			case IR_CALL:
				sprintf(s, "%s = CALL .%s(", c->var1->name, c->var0->name);
				fputs(s, ir_file);
				param = c->var0->function->first;
				for (int i = 1; i < c->var0->function->paramCnt; i++) {
					if (i == c->var0->function->paramCnt) {
						sprintf(s, "%s", param->name);
						fputs(s, ir_file);
					} else {
						sprintf(s, "%s, ", param->name);
						fputs(s, ir_file);
						param = param->next;
					}
				}
				sprintf(s, ") GOTO %s", c->var0->name);
				fputs(s, ir_file);
				break;
			case IR_ASSIGN_ARR:
				sprintf(s, "%s = %s[%s]", c->var0->name, c->var1->name,
						c->var1->position);
				fputs(s, ir_file);
				break;
			case IR_ARR_ASSIGN:
				sprintf(s, "%s[%s] = %s", c->var0->name, c->var0->position,
						c->var1->name);
				fputs(s, ir_file);
				break;
			case IR_FUNC_START:
				if (c->var0->type == 4) {
					sprintf(s, "%s:", c->var0->name);
					fputs(s, ir_file);
				}
				tab = '\t';
				break;
			case IR_FUNC_END:
				break;
			default:
				sprintf(s, "Error at: %d", i);
				fputs(s, ir_file);
			}

			if (c->op == IR_WHILE_BEGIN || c->op == IR_DO_WHILE_BEGIN)
				tab = '\t';
			if (c->op != IR_PARA && c->op != IR_WHILE_BEGIN && c->op
					!= IR_DO_WHILE_BEGIN) {
				sprintf(s, "\n");
				fputs(s, ir_file);
			}
		}
	}
}
