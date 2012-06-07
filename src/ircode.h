

DO WHILE

enum op_codes {
	IR_DO_WHILE, IR_IF_ELSE, IR_RETURN, IR_ASSIGN, IR_LOGICAL_OR, IR_LOGICAL_AND, IR_EQ, IR_NE, IR_LS, IR_LSEQ,
	IR_GTEQ, IR_GT, IR_SHIFT_LEFT, IR_SHIFT_RIGHT, IR_PLUS, IR_MINUS, IR_MUL, IR_LOGICAL_NOT, IR_UNARY_MINUS,
	IR_UNARY_PLUS
};

struct ir_struct {
	struct var *var0;
	struct var *var1;
	int label;
	int jmp;
};
