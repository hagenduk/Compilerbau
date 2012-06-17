#ifndef _DHBWCC_IRCODE_H
#define _DHBWCC_IRCODE_H


enum op_codes {
	IR_DO_WHILE, IR_IF_ELSE, IR_RETURN, IR_ASSIGN, IR_LOGICAL_OR, IR_LOGICAL_AND, IR_EQ, IR_NE, IR_LS, IR_LSEQ,
	IR_GTEQ, IR_GT, IR_SHIFT_LEFT, IR_SHIFT_RIGHT, IR_PLUS, IR_MINUS, IR_MUL, IR_LOGICAL_NOT, IR_UNARY_MINUS,
	IR_UNARY_PLUS, IR_ARR_INIT, IR_ARR_ASSIGN, IR_FUNC_START, IR_FUNC_END, IR_POINT, IR_PARA, IR_GOTO, IR_WHILE_BEGIN,
	IR_WHILE_END, IR_IF, IR_ASSIGN_ARR, IR_DO_WHILE_BEGIN, IR_CALL
}op_codes;

struct ir_struct {
	enum op_codes op;
	struct entry *var0;
	struct entry *var1;
	struct entry *var2;
	int label;
	int jmp;
} ir_struct;

void ir_entry(enum op_codes op, entry *var0, entry *var1, entry *var2, int jmp);
void ir_entry_assign(entry *var0, entry *var1);
struct entry *ir_1exp(enum op_codes op, struct entry *var0);
struct entry *ir_2exp(enum op_codes op, struct entry *var1,
		struct entry *var2);
void ir_return(enum op_codes op, struct entry *var0);
struct entry *ir_assign_arr(struct entry *var0, struct entry *var1);
void ir_if(struct entry *var0);
void ir_goto();
void ir_while(struct entry *var0);
void ir_while_begin();
void ir_while_goto_begin();
void ir_do_while_begin();
void ir_do_while_end(struct entry *var0);
struct entry *ir_funccall(struct entry *var0, int jmp);
void backp_if(int new);
void backp_return();
void backp_while();
int ir_find_FuncDef(struct entry *var0);
struct entry *ir_tmp();
int set_jmpLabel(int pos, int label);
void ir_set_file(FILE *file);
void generate_ir_code();

#endif
