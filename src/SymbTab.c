#include <stdio.h>
#include <stdlib.h>
#include "SymbTab.h"


struct entry *localentryptr;
struct SymTab *root;

//void initialise
struct SymTab *init_table(){
 struct SymTab *rootptr;     //root table erstellen
 rootptr = (struct SymTab *) malloc (sizeof (struct SymTab));
 rootptr->father=NULL;
 rootptr->start=NULL;
 rootptr->id=0;
 root=rootptr;
 return rootptr;
}

//void newfunction
struct SymTab *decfunction(struct SymTab *parenttable, char const *name){
  localentryptr=new_entry(parenttable,0,name,0,0,0);
  struct SymTab *localsmybtabptr;     //root table erstellen
  localsmybtabptr = (struct SymTab *) malloc (sizeof (struct SymTab)); 
  localentryptr->function=new_function(parenttable);
  localsmybtabptr=localentryptr->function;
  return localsmybtabptr;              
}



//void new_entry
void new_entry(struct SymTab *current, int offset, char const *name, int scope, int type, int position){

struct entry *currententry; //link zu aktuellem eintrag

     if(current->start==NULL){               //wenn in der aktuellen symboltabelle noch kein eintrag existiert
      struct entry *start;                      //erstelle Eintrag
      start = (struct entry *) malloc (sizeof (struct entry));
      current->start=start;             //schreibe an start
      start->next=NULL;                 //initialisiere
      start->previous=NULL;             //...
      start->id=0;                      //id auf 0 da erster Eintrag
      currententry=current->start;      //setze aktuellen eintrag auf neuen eintrag
     }
     else{                                    //Ansonsten
          currententry=current->start;        //setzte aktuellen auf 1. Eintrag in tabelle
          while(currententry->next!=NULL){          //solange der eeintrag weiter in next hat
          currententry=currententry->next;    //setze aktuellen eintrag auf nexteintrag
          }
       struct entry *newlast;                        //erstelle neuen eintrag
       newlast = (struct entry *) malloc (sizeof (struct entry));
       currententry->next=newlast;            //aktueler eintrag.next (vorher null) jetzt neuer Eintrag
       newlast->previous=currententry;         //neuer eintrag.vorherige=aktueller Eintrag
       newlast->id=currententry->id+1;        //id = id des aktuellen +1
       newlast->next=NULL;                 //initialisiere
       currententry=newlast;                  //aktueller = neuer eintrag
     }
     currententry->offset=offset;             //setzte werte aus übergabe für aktuellen eintrag
     currententry->name = (char *) malloc (strlen (name) + 1);
     strcpy (currententry->name,name);
     currententry->scope=scope;
     currententry->type=type;
     currententry->function=NULL;
     //gib das Ergebnis aus
     printf("Inserted into Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'type' %d \n }", current->id, currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->type);
}

//void neue function, eröffnet sub level
struct SymTab *new_function(struct SymTab *current){
 struct SymTab *neuetabelle;                    //Neue Tabelle erstellen
 neuetabelle = (struct SymTab *) malloc (sizeof (struct SymTab));
 neuetabelle->father=current;           //Vater speichern
 neuetabelle->start=NULL;               //start auf NULL
 neuetabelle->id=neuetabelle->father->id+1;                   //id hochzählen (kann mehrere tabellen mit gleicher id geben wenn gleicher Vater)
 return neuetabelle;                    //rückgabe aktueller tabelle
}

//end_function um zum top level zurück zu kehren
struct SymTab *end_function(struct SymTab *current){
       return current->father;          //returns father of SymTab
}

int printallstart(char *file){     
FILE* datei;
datei=fopen(file,"w+");
/* alternativ zu r+
r - nur zum lesen
w - nur zum schreiben
r+, w+ - zum schreiben UND lesen (ueberschreiben der datei)
a - schreiben, aber anhaengen an die datei
a+ - schreiben und lesen, an die datei wird angehaengt*/
if(datei==NULL)
//fehler beim oeffnen
return -1;
fseek(datei,0,SEEK_END);
/*fseek veraendert die position in der datei...
SEEK_END heisst ans ende der datei, SEEK_SET ist der anfang und SEEK_CUR ist die aktuelle position...
0 ist der wert um den die position geaendert wird (in unserem fall 0, da wir ja das datei einde wollen)
*/
printall(datei, root);

    fclose(datei);                     //wichtig: FILE* muss wieder geschlossen werden

}

void printall(FILE* datei, struct SymTab *root){
    struct entry *current_entry;   
    current_entry = (struct entry *) malloc (sizeof (struct entry));
    current_entry = root->start;
         while(current_entry!=NULL){                    //While current entry exists (last entry is null=false, Loop1)
           if(current_entry->function!=NULL){            //If it is a pointer to new symboltable
           printall(datei, current_entry->function);
           }
           printentry(current_entry, datei);
           current_entry=current_entry->next;
         }                                            //end loop 1 

}

void printentry(struct entry *currententry, FILE* datei){
fprintf(datei,"Contains entry %d\n{'offset' %d, 'name' %s, 'scope' %d, \n 'type' %d \n }", currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->type);      //wie printf() zu handhaben!
}

struct entry *get_name(struct SymTab *current, char const *name){
   struct entry *return_entry;                       //initialise returnentry with null, 
   return_entry = (struct entry *) malloc (sizeof (struct entry));
   return_entry=NULL;
   struct entry *current_entry;            //set current entry to start entry of current SymTab             
   current_entry = (struct entry *) malloc (sizeof (struct entry));
   current_entry=current->start;
   while(current->father!=NULL || current_entry!=NULL){       //While Father of current Symtab exists or current entry exits (Loop1)
            //printf("\n Looking in Table id %d \n", current->id);
         while(current_entry!=NULL){                    //While current entry exists (last entry is null=false, Loop2)
         //printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray); 
           if((strcmp (current_entry->name,name) == 0)){            //If name = currententry->name
           return_entry=current_entry;             //returnentry=currententry
           break;                                  //break loop1
           }                                    //else
           current_entry=current_entry->next;       //current entry is current_entry->next (next entry in same SymTab)           
         }                                       //end loop 2
       if(return_entry!=NULL) break;          
       if(current->father!=NULL){                    // if entry yet found break loop 2
       current=current->father;                 //set current SymTab to current->father
       current_entry=current->start;}         //current entry = start of symtab
      //printf("looking next");
   }
   if(return_entry==NULL) printf("Wert: STIRB!");                                                //end loop 1
   return return_entry;                           //return returnentry (null if not found)
//printf("ende");
}