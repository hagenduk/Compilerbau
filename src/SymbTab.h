#ifndef _DHBWCC_SYMTABLE_H
#define _DHBWCC_SYMTABLE_H

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
   int scope;                 // 0 = local, else different
   struct entry *previous;    // previous entry
   struct entry *next;        // next entry
   int type;               //is it an array
   int position;              //if yes whats the position?
   struct SymTab *function;   //Functionpointer
   int retType;               //if function returntype
   int param;                 // Number of the parameters
} entry;


//Struct Symbtab
typedef struct SymTab {
	struct entry *start;   // starting entry
	struct SymTab *father;       // father table
	int id;
} SymTab;


struct SymTab *init_table();
void new_entry(struct SymTab *current, int offset, char const *name, int scope, int type, int position);
struct SymTab *new_function(struct SymTab *current);
struct SymTab *end_function(struct SymTab *current);
struct entry *get_name(struct SymTab *current, char const *name);
void printall(FILE* datei, struct SymTab *root);
void printentry(struct entry *currententry, FILE* datei); 
#endif
