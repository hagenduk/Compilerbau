#include <stdio.h>
#include <stdlib.h>
#include "SymbTab.h"

/*
struct entry *localentryptr;
struct SymbTab *localtableptr;

//for testing
int main(int argc, char *argv[])
{
  struct entry *currententry;     //root table erstellen
  currententry = (struct entry *) malloc (sizeof (struct entry));
  
  struct SymTab *rootptr;     //root table erstellen
  rootptr = (struct SymTab *) malloc (sizeof (struct SymTab));
  rootptr=init_table();
  new_entry(rootptr,5,"hallo",0,0,0);
  rootptr=new_function(rootptr);
  printf("\n Father is: %s \n", rootptr->father);
  currententry=get_name(rootptr,"hallo");
  printf("Read from Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", rootptr->id, currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->isarray); 
  new_entry(rootptr,1,"hallo",2,3,4);
  currententry=get_name(rootptr,"hallo");
  printf("Read from Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", rootptr->id, currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->isarray); 
  rootptr=end_function(rootptr);
  currententry=get_name(rootptr,"hallo");
  printf("Read from Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", rootptr->id, currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->isarray); 
  system("PAUSE");  
  return 0;
}
*/
//void initialise
struct SymTab *init_table(){
 struct SymTab *rootptr;     //root table erstellen
 rootptr = (struct SymTab *) malloc (sizeof (struct SymTab));
 rootptr->father=NULL;
 rootptr->start=NULL;
 rootptr->id=0;
 return rootptr;
}

//void new_entry
void new_entry(struct SymTab *current, int offset, char const *name, int scope, int isarray, int position){

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
     currententry->isarray=0;
     //gib das Ergebnis aus
     printf("Inserted into Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, currententry->id, currententry->offset, currententry->name, currententry->scope, currententry->isarray);
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

//enty read_entry by name
struct entry *get_name(struct SymTab *current, char const *name){
   struct entry *return_entry=NULL;                       //initialise returnentry with null, 
   struct entry *current_entry;            //set current entry to start entry of current SymTab             
   current_entry = (struct entry *) malloc (sizeof (struct entry));
   current_entry=current->start;
   while(current->father!=NULL || current_entry!=NULL){       //While Father of current Symtab exists or current entry exits (Loop1)
            printf("\n Looking in Table id %d \n", current->id);
         while(current_entry!=NULL){                    //While current entry exists (last entry is null=false, Loop2)
         printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray); 
           if((strcmp (current_entry->name,name) == 0)){            //If name = currententry->name
           return_entry=current_entry;             //returnentry=currententry
           break;                                  //break loop1
           }
           else{                                    //else
           current_entry=current_entry->next;       //current entry is current_entry->next (next entry in same SymTab)
           }
         }                                        //end loop 2
       if(return_entry) break;                    // if entry yet found break loop 2
       current=current->father;                 //set current SymTab to current->father
       current_entry=current->start;         //current entry = start of symtab
      printf("looking next");
   }                                              //end loop 1
   return return_entry;                           //return returnentry (null if not found)
printf("ende");
}
