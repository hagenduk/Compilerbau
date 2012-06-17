#include <stdio.h>
#include <stdlib.h>
#include "SymbTab.h"


struct entry *localentryptr;		//aktueller func entry in father
struct SymTab *root;				//root


/**
 * \brief Erstellt und initialisiert eine neue Symboltabelle.
 * \return Pointer auf neuerstellte Symboltabelle.
 */
struct SymTab *init_table(){
	struct SymTab *rootptr;     //root table erstellen
	rootptr = (struct SymTab *) malloc (sizeof (struct SymTab));
	rootptr->father=NULL;
	rootptr->start=NULL;
	rootptr->id=0;
	root=rootptr;

	return rootptr;
}


/**
 * \brief Anlegen einer neuen Funktion
 * \param parenttable	Aktuelle Symboltabelle
 * \param name			Name der Funktion
 * \param type			Prototype -> 3 ; Function -> 4
 * \param returnType	Returntype der Funktion: 0 -> void; 1 -> int
 * \return 				Pointer auf die Symboltabelle der neuen Funktion.
 */
// TODO Marvin: Retrun-Type kann nicht gesetzt werden
struct SymTab *decfunction(struct SymTab *parenttable, char const *name, int type, int returnType){
  localentryptr=new_entry(parenttable,0,name,0,type,0);
  struct SymTab *localsymbtabptr;     //root table erstellen
  localsymbtabptr = (struct SymTab *) malloc (sizeof (struct SymTab));
  localentryptr->function=new_function(parenttable);
  localsymbtabptr=localentryptr->function;
  localsymbtabptr->returntype = returnType;
  return localsymbtabptr;
}


/**
 * \brief Erstellt einen neuen Eintrag und fuegt diesen der Tabelle "current" hinzu.
 * \param current		Tabelle, der der Eintrag hinzugefügt werden soll.
 * \param offset		Laenge des Eintrages (siehe Beschreibung der Entry Struktur)
 * \param name			Name des Identifiers
 * \param scope			Lokale Variable, oder Parameter (siehe Beschreibung der Entry Struktur)
 * \param type			Typ des Identifiers  (siehe Beschreibung der Entry Struktur)
 * \param position		(siehe Beschreibung der Entry Struktur)
 * \return				Gibt den neu angelegten Entry zurueck.
 */
struct entry *new_entry(struct SymTab *current, int offset, char const *name, int scope, int type, int position){
	struct entry *currententry;											//link zu aktuellem eintrag

	if(current->start==NULL){               							//wenn in der aktuellen symboltabelle noch kein eintrag existiert
		struct entry *start;                      						//erstelle Eintrag
		start = (struct entry *) malloc (sizeof (struct entry));

		current->start=start;             								//schreibe an start
		start->next=NULL;                 								//initialisiere
		start->previous=NULL;             								//...
		start->id=0;                      								//id auf 0 da erster Eintrag

		currententry=current->start;      								//setze aktuellen eintrag auf neuen eintrag
	} else {                                   							//Ansonsten
    	 currententry=current->start;        							//setzte aktuellen auf 1. Eintrag in tabelle

    	 while(currententry->next!=NULL) {          					//solange der eeintrag weiter in next hat
			currententry=currententry->next;    							//setze aktuellen eintrag auf nexteintrag
    	 }

    	 struct entry *newlast;                        					//erstelle neuen eintrag
    	 newlast = (struct entry *) malloc (sizeof (struct entry));

    	 currententry->next=newlast;            						//aktueler eintrag.next (vorher null) jetzt neuer Eintrag
    	 newlast->previous=currententry;         						//neuer eintrag.vorherige=aktueller Eintrag
    	 newlast->id=currententry->id+1;        						//id = id des aktuellen +1
    	 newlast->next=NULL;                 							//initialisiere

			currententry=newlast;                  						//aktueller = neuer eintrag
	}

	currententry->offset=offset;             							//setzte werte aus übergabe für aktuellen eintrag
	currententry->position = 0;

	currententry->name = (char *) malloc (strlen (name) + 1);
	strcpy (currententry->name,name);

	currententry->scope=scope;
	currententry->type=type;
	currententry->function=NULL;


     //gib das Ergebnis aus
     printf("\tInserted into Table %d \n\t in entry %d VALUES \n\t{'offset' %d, 'name' %s, 'scope' %d, \n\t 'type' %d \n\t }\n\n",
    		 current->id,
    		 currententry->id,
    		 currententry->offset,
    		 currententry->name,
    		 currententry->scope,
    		 currententry->type);


     return currententry;
}

/**
 * \brief Erstellt einen neuen Function Parameter.
 * \param current		Vater der Tabelle, der der Eintrag hinzugefügt werden soll.
 * \param name			Name des Identifiers
 * \param type			Typ des Identifiers
 * \return				1 =Erfolg 0=Error
 */
void new_param(struct SymTab *current, char const *name, int type){
	struct param *currentparam;											//link zu aktuellem eintrag

	if(current->first==NULL){               							//wenn in der aktuellen symboltabelle noch kein eintrag existiert
		struct param *start;                      						//erstelle Eintrag
		start = (struct param *) malloc (sizeof (struct param));

		current->first=start;             								//schreibe an start
		start->next=NULL;                 								//initialisiere
		start->previous=NULL;             								//...
		start->id=0;                      								//id auf 0 da erster Eintrag

		currentparam = start;   										//setze aktuellen eintrag auf neuen eintrag
	} else {                                   							//Ansonsten
    	 currentparam=current->first;        							//setzte aktuellen auf 1. Eintrag in tabelle

    	 while(currentparam->next!=NULL) {          					//solange der eeintrag weiter in next hat
			currentparam=currentparam->next;    							//setze aktuellen eintrag auf nexteintrag
    	 }

    	 struct param *newlast;                        					//erstelle neuen eintrag
    	 newlast = (struct param *) malloc (sizeof (struct param));

    	 currentparam->next=newlast;            						//aktueler eintrag.next (vorher null) jetzt neuer Eintrag
    	 newlast->previous=currentparam;         						//neuer eintrag.vorherige=aktueller Eintrag
    	 newlast->id=currentparam->id+1;        						//id = id des aktuellen +1
    	 newlast->next=NULL;                 							//initialisiere

			currentparam=newlast;                  						//aktueller = neuer eintrag
	}

	currentparam->name = (char *) malloc (strlen (name) + 1);
	strcpy (currentparam->name,name);

	currentparam->type=type;

     //gib das Ergebnis aus
     printf("\tInserted into Table %d \n\t in entry %d VALUES \n\t{'name' %s, \n\t 'type' %d \n\t }\n\n",
    		 current->id,
    		 currentparam->id,
    		 currentparam->name,
    		 currentparam->type);

}

/**
 * \brief 			Prüft ob paramname local bereits vergeben ist
 * \param current	Symboltabelle, die verglichen werden soll.
 * \param name		Name des Parameters.
 * \return			Integer 1 - Ja, 0 - Nein
 */
struct param *exists_param(struct SymTab *current, char const *name){

	struct param *found_param;                       						//initialise returnparam with null,
		found_param = (struct param *) malloc (sizeof (struct param));
		found_param=NULL;

		struct param *current_param;            								//set current param to start param of current SymTab
		current_param = (struct param *) malloc (sizeof (struct param));
		current_param=current->first;

			while(current_param!=NULL){                    						//While current param exists (last param is null=false, Loop2)

				//printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray);

				if((strcmp (current_param->name,name) == 0)){            		//If name = currentparam->name
					found_param=current_param;             					//returnparam=currentparam
					break;//break loop1
				}

				current_param=current_param->next;       						//current entry is current_param->next (next param in same SymTab)
			}//end loop 2

	if(found_param!=NULL) return found_param;
	return NULL;
}

/**
 * \brief Gibt den Typ eines Parameters zurueck,
 * \param current Symboltabelle der Function
 * \param index Position des Parameters (beginnt bei 0)
 * \return Type des Parameters, oder -1 falls es an der Stelle kein Parameter exisitiert
 */
int getTypeOfParam(struct SymTab *current, int index) {
	struct param *p;
	p = current->first;

	if(p == NULL) {
		return -1;
	}

	if(index == 0) {
		return p->type;
	}

	if(index>0) {
		for(int i=0; i<=index; i++) {
			if( p->next == NULL ) {
				printf("NEEEEEEEEEEEEEEEEIN\n\n");
				return -1;
			}
			p = p->next;
		}
	}

	return p->type;
}

/**
 * \brief Erstellt eine neue Symboltabelle und verkettet sie mit der aktuellen Tabelle "current"
 * \param current	Aktuelle Symboltabelle
 * \return			Pointer auf neuangelegte Symboltabelle
 */
struct SymTab *new_function(struct SymTab *current){
	struct SymTab *neuetabelle;                    						//Neue Tabelle erstellen
	neuetabelle = (struct SymTab *) malloc (sizeof (struct SymTab));
	neuetabelle->father=current;           								//Vater speichern
	neuetabelle->start=NULL;               								//start auf NULL
	neuetabelle->id=neuetabelle->father->id+1;                  		//id hochzählen (kann mehrere tabellen mit gleicher id geben wenn gleicher Vater)

	return neuetabelle;                    								//rückgabe aktueller tabelle
}


/**
 * \brief Liefert die uebergeordnete Symboltabelle zurueck.
 * \param current	Aktuelle Symboltabelle
 * \return 			Pointer auf uebergeordnete Symboltabelle
 */
struct SymTab *end_function(struct SymTab *current, int numberOfParameters){

	struct entry *function;
	localentryptr->param=numberOfParameters;

	current->paramCnt = numberOfParameters;
	return current->father;          //returns father of SymTab
}


/**
 * \brief Oeffnet eine Datei zum schreiben und uebergint das handle an printall
 * \return	Statuscode, ob Prozess erfolgreich war
 * 			-1 -> Datei konnte nicht geoeffnet werden.
 * 			 1 -> Schreibprozess wurde erfolgreich beendet.
 */
int printallstart(char *file){     
	FILE* datei;
	datei=fopen(file,"w");					/*	alternativ zu r+
												r - nur zum lesen
												w - nur zum schreiben
												r+, w+ - zum schreiben UND lesen (ueberschreiben der datei)
												a - schreiben, aber anhaengen an die datei
												a+ - schreiben und lesen, an die datei wird angehaengt
											*/

	if(datei==NULL)	return -1;				//fehler beim oeffnen

	printall(datei, root);

    fclose(datei);                     		//wichtig: FILE* muss wieder geschlossen werden

    return 1;

}


/**
 * \brief Schreibt alle Eintraege einer (Root-)Symboltabelle und deren untergeordneten Symboltabellen in eine Datei.
 * \param datei	Handle auf die Datei, mit Schreibrechten
 * \param root	Root-Symboltabelle
 */
void printall(FILE* datei, struct SymTab *root){
	struct entry *current_entry;
    current_entry = (struct entry *) malloc (sizeof (struct entry));
    current_entry = root->start;

    while(current_entry!=NULL){                    							//While current entry exists (last entry is null=false, Loop1)

    	if(current_entry->function!=NULL){            						//If it is a pointer to new symboltable
    		printall(datei, current_entry->function);
    	}

    	printentry(current_entry, datei);
    	current_entry=current_entry->next;
    }//end loop 1
}


/**
 * \brief Schreibt einen Eintrag formatiert in eine Datei.
 * \param currententry	Pointer auf den Eintrag
 * \param datei			Handle auf die Datei, mit Schreibrechten
 */
void printentry(struct entry *currententry, FILE* datei){
	fprintf(datei,"Contains entry %d\n{'offset' %d, 'name' %s, 'scope' %d, \n 'type' %d \n }",
			currententry->id,
			currententry->offset,
			currententry->name,
			currententry->scope,
			currententry->type);
}




/**
 * \brief 			Prüft ob aktuelle Tabelle root ist
 * \param current	Symboltabelle, die verglichen werden soll.
 * \return			Integer 1 - Ja, 0 - Nein
 */
int is_root_table(struct SymTab *current){
	if(current==root) return 1;
	return 0;
}

/**
 * \brief 			Prüft ob name local bereits vergeben ist
 * \param current	Symboltabelle, die verglichen werden soll.
 *  * \param name		Name des Eintrages.
 * \return			Integer 1 - Ja, 0 - Nein
 */
int exists_entry(struct SymTab *current, char const *name){

	struct entry *found_entry;                       						//initialise returnentry with null,
		found_entry = (struct entry *) malloc (sizeof (struct entry));
		found_entry=NULL;

		struct entry *current_entry;            								//set current entry to start entry of current SymTab
		current_entry = (struct entry *) malloc (sizeof (struct entry));
		current_entry=current->start;

			while(current_entry!=NULL){                    						//While current entry exists (last entry is null=false, Loop2)

				//printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray);

				if((strcmp (current_entry->name,name) == 0)){            		//If name = currententry->name
					found_entry=current_entry;             					//returnentry=currententry
					break;//break loop1
				}

				current_entry=current_entry->next;       						//current entry is current_entry->next (next entry in same SymTab)
			}//end loop 2

	if(found_entry!=NULL) return 1;
	return 0;
}

/**
 * \brief Sucht in einer Symboltabelle und ggf. in der uebergeordneten Symboltabelle nach einem Eintrag. Der Eintrag wird mit Hilfe des Names gesucht.
 * \param current	Symboltabelle, in der gesucht werden soll.
 * \param name		Name des Eintrages.
 * \return			Pointer auf die Funktionss.
 */
struct SymTab *get_function(struct SymTab *current, char const *name){

	struct entry *found_entry;                       						//initialise returnentry with null,
		found_entry = (struct entry *) malloc (sizeof (struct entry));
		found_entry=NULL;

		struct entry *current_entry;            								//set current entry to start entry of current SymTab
		current_entry = (struct entry *) malloc (sizeof (struct entry));
		current_entry=current->start;

			while(current_entry!=NULL){                    						//While current entry exists (last entry is null=false, Loop2)

				//printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray);

				if((strcmp (current_entry->name,name) == 0 && current_entry->function!=NULL)){            		//If name = currententry->name and function pointer not NULL
					found_entry=current_entry;             					//returnentry=currententry
					break;//break loop1
				}

				current_entry=current_entry->next;       						//current entry is current_entry->next (next entry in same SymTab)
			}//end loop 2

	if(found_entry!=NULL) return NULL;
	return found_entry->function;												//return function
}



/**
 * \brief Sucht in einer Symboltabelle und ggf. in der uebergeordneten Symboltabelle nach einem Eintrag. Der Eintrag wird mit Hilfe des Names gesucht.
 * \param current	Symboltabelle, in der gesucht werden soll.
 * \param name		Name des Eintrages.
 * \return			Pointer auf das zuerst gefundene Entry.
 */
struct entry *get_name(struct SymTab *current, char const *name){
	struct entry *return_entry;                       						//initialise returnentry with null,
	return_entry = (struct entry *) malloc (sizeof (struct entry));
	return_entry=NULL;

	struct entry *current_entry;            								//set current entry to start entry of current SymTab
	current_entry = (struct entry *) malloc (sizeof (struct entry));
	current_entry=current->start;

	while(current->father!=NULL || current_entry!=NULL){      				//While Father of current Symtab exists or current entry exits (Loop1)

		//printf("\n Looking in Table id %d \n", current->id);

		while(current_entry!=NULL){                    						//While current entry exists (last entry is null=false, Loop2)

			//printf("\n Lookin in Table %d \n in entry %d VALUES \n{'offset' %d, 'name' %s, 'scope' %d, \n 'isarray' %d \n }", current->id, current_entry->id, current_entry->offset, current_entry->name, current_entry->scope, current_entry->isarray);

			if((strcmp (current_entry->name,name) == 0)){            		//If name = currententry->name
				return_entry=current_entry;             					//returnentry=currententry
				break;//break loop1
			}

			current_entry=current_entry->next;       						//current entry is current_entry->next (next entry in same SymTab)
		}//end loop 2

		if(return_entry!=NULL) break;

		if(current->father!=NULL){                    						//if entry yet found break loop 2
			current=current->father;                 						//set current SymTab to current->father
			current_entry=current->start;         							//current entry = start of symtab
		}
		//printf("looking next");
	}//end loop 1

	if(return_entry==NULL) printf("Wert: STIRB!");


	return return_entry;                           //return returnentry (null if not found)
	//printf("ende");
}

struct SymTab *get_rootptr() {
	return root;
}

int getReturnType(struct SymTab *current) {
	return current->returntype;
}

int getParamCnt(struct SymTab *current) {
	return current->paramCnt;
}

struct entry *getParamAsEntry(struct param *p) {
	printf("----getParamAsEntry");
	struct entry *e;
	e = (struct entry *) malloc (sizeof (struct entry));

	e->id = p->id;
	e->value = p->value;

	switch(p->type) {
		case 0:
			e->type = 0;
			break;
		case 1:
			e->type = 1;
			break;
		case 2:
			e->type = 2;
			break;
		default:
			printf("ERROR: Check type of parameters!.\n");
	}

	return e;
}

/*************************************************************   TypSystem    ***************************************************/

int checktype(int first, int second){
if(first==second) return 1;
return 0;
}

int checkexpr(struct entry *first){
	if(first->function!=NULL){
		if(getReturnType(first->function)==0){
				return 0;
		}
	 }
	return 1;

}

