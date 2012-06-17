#ifndef _DHBWCC_SYMBTAB_H
#define _DHBWCC_SYMBTAB_H

//Struct entry, Offset, name, typ, anzahl der übergabeparam, level(local,anders)
/*Types: IMMER: name, type, offset
        0 - void
        1 - int
        2 - array
        3 - prototype
        4 - function
*/
typedef struct entry {
   int id;                    //just for testing
   int offset;                // length of the entry
   char *name;                 //name of the object in the entry
   int scope;                 // 0 = local, else different (parameter)
   struct entry *previous;    // previous entry
   struct entry *next;        // next entry
   int type;               //is it an array
   int value;              //int value
   int position;			// array position
   struct SymTab *function;   //Functionpointer
   int param;                 // Number of the parameters
} entry;


//Struct param, name, typ
/*Types: IMMER: name, type, offset
        0 - void
        1 - int
        2 - array
        3 - prototype
        4 - function
*/
typedef struct param {
   int id;                    //just for testing
   char *name;                 //name of the object in the entry
   struct param *previous;    // previous entry
   struct param *next;        // next entry
   int value;					//value
   int position;			// array position
   int type;               //is it an array
} param;



//Struct Symbtab
/*Returntypes
        0 - void
        1 - int
        2 - array
*/
typedef struct SymTab {
	struct entry *start;   // starting entry
	struct SymTab *father;       // father table
	struct param *first;		//first param of Function (each Table = 1 Function)
	int paramCnt;				// Anzahl der Parameter
	int id;
	int returntype;
} SymTab;


struct SymTab *init_table();
struct entry *new_entry(struct SymTab *current, int offset, char const *name, int scope, int type, int position);
struct SymTab *new_function(struct SymTab *current);
struct SymTab *end_function(struct SymTab *current, int number);
struct entry *get_name(struct SymTab *current, char const *name);
int printallstart(char *file);
struct SymTab *decfunction(struct SymTab *parenttable, char const *name, int type, int returnType);
int is_root_table(struct SymTab *current);
int exists_entry(struct SymTab *current, char const *name);
struct SymTab *get_function(struct SymTab *current, char const *name);
void new_param(struct SymTab *current, char const *name, int type);
struct param *exists_param(struct SymTab *current, char const *name);
int getTypeOfParam(struct SymTab *current, int pos);
struct SymTab *get_rootptr();
int getReturnType(struct SymTab *current);
//struct entry *getParamAsEntry(struct param *p);

#endif
