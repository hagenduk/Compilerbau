#ifndef _DHBWCC_IRCODE_H
#define _DHBWCC_IRCODE_H


enum op_codes {
	IR_DO_WHILE, IR_IF_ELSE, IR_RETURN, IR_ASSIGN, IR_LOGICAL_OR, IR_LOGICAL_AND, IR_EQ, IR_NE, IR_LS, IR_LSEQ,
	IR_GTEQ, IR_GT, IR_SHIFT_LEFT, IR_SHIFT_RIGHT, IR_PLUS, IR_MINUS, IR_MUL, IR_LOGICAL_NOT, IR_UNARY_MINUS,
	IR_UNARY_PLUS, IR_ARR_INIT, IR_ARR_ASSIGN, IR_FUNC_START, IR_FUNC_END, IR_POINT, IR_PARA
};

struct ir_struct {
	enum op_codes op;
	struct entry *var0;
	struct entry *var1;
	struct entry *var2;
	int label;
	int jmp;
} ir_struct;

