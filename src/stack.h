#include "SymbTab.h"
typedef struct SymTab *stack_data;

extern void stack_init();
/* Initializes this library.
   Call first before calling anything. */

extern void stack_clear();
/* Clears the stack of all entries. */

extern int stack_empty();
/* Returns 1 if the stack is empty, 0 otherwise. */

extern void stack_push(stack_data d);
/* Pushes the value d onto the stack. */

extern stack_data stack_pop();
/* Returns the top element of the stack,
   and removes that element.
   Returns garbage if the stack is empty. */
