	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                             Úpravy: Bohuslav Køena, øíjen 2013
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
//inicializujem si zoznam(nastavenim prveho aktualneho a posledneho prvnu na NULL)
    L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;

}

void DLDisposeList (tDLList *L)	{
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
/* prejde zoznam a uvolni prvky */
	tDLElemPtr act= L->First; //vytvorim si premenuu pre prechod zoznamom v mojom pripade si ju nazvem act(aktualny prvok)
	tDLElemPtr hlp; //vytvorim si pomocnu premennu (nazval som si ju hlp(help)
//prechadzanie zoznamom a rusenie jeho prvkov
		while(act != NULL){
			hlp=act;
			act = act->rptr;
			free(hlp);
		}
//uvedie zoznam do povodneho stavu pomocou dopredu naimplementovanej funkcie DLinitlist
	DLInitList(L);	
}

void DLInsertFirst (tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
//alokujem si priestor v pamaty a vytvorim novy prvok(new)
	tDLElemPtr new = malloc(sizeof(struct tDLElem));
//test ci sa alokacia podarila
	if(new == NULL) {
		DLError();
		return;
	}
//nastavim novy prvok (jeho hodnotu , lavy a pravy ukazatel)
	new->data = val;
	new->lptr = NULL;
	new->rptr = L->First; 
//ak nieje zoznam prazdny tak nastavy lavy ukazatel prveho prvku na novy prvok
	if(L->First != NULL)
		L->First->lptr = new; 
//ak je zoznam prazdny tak novy prvok bude aj prvy aj posledny	
	else
		L->Last = new;
	L->First = new;

}

void DLInsertLast(tDLList *L, int val)	{
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
//alokujem si priestor v pamaty a vytvorim novy prvok(new)
	tDLElemPtr new = malloc(sizeof(struct tDLElem));
//test ci sa alokacia podarila
	if(new == NULL) {
		DLError();
		return;
	}
//nastavim novy prvok (jeho hodnotu , lavy a pravy ukazatel)
	new->data = val;
	new->rptr = NULL;
	new->lptr = L->Last;
//ak nieje zoznam prazdny tak nastavy lavy ukazatel prveho prvku na novy prvok
	if(L->Last != NULL)
		L->Last->rptr = new;
//ak je zoznam prazdny tak novy prvok bude aj prvy aj posledny			
	else
		L->First = new;
	L->Last = new;
	
}

void DLFirst (tDLList *L)	{
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
//nastavenie aktivity(act) na prvy prvok
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
//nastavenie aktivity(act) na posledny prvok
	L->Act = L->Last;	
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
//zistenie ci je zoznam prazdny
	if(L->First == NULL){
		DLError();
		return;
	}
//do val ulozi data prveho prvku
	*val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
//zistenie ci je zoznam prazdny
	if(L->First == NULL){
		DLError();
		return;
	}
//do val ulozi data prveho prvku
	*val = L->Last->data;
}

void DLDeleteFirst (tDLList *L)	{
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
//zistenie ci je zoznam prazdny ak je tak sa nic nedeje
	if(L->First == NULL)
		return;
//vytvorim si pomocnu premennu hlp(help)
	tDLElemPtr hlp;
	hlp = L->First;
//ak bol aktivny prvy aktivita sa straca (== NULL)
	if(L->Act == L->First)
		L->Act = NULL;
//ak je zoznam prazdny, nic sa nedeje	
	if(L->First == L->Last){
		L->Last = NULL;
		L->First = NULL;
	}
//zrusi prvy prvok zo zoznamu
	else{
		L->First = L->First->rptr;
		L->First->lptr = NULL;
	}
	free(hlp);
}	

void DLDeleteLast (tDLList *L)	{
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
//zistenie ci je zoznam prazdny ak je tak sa nic nedeje
	if(L->Last == NULL)
		return;
//vytvorim si pomocnu premennu hlp(help)
	tDLElemPtr hlp;
	hlp = L->Last;
//ak bol aktivny posledny aktivita sa straca (== NULL)
	if(L->Act == L->Last)
		L->Act = NULL;
//ak je zoznam prazdny, nic sa nedeje		
	if(L->Last == L->First){
		L->First = NULL;
		L->Last = NULL;
	}
//zrusi posledny prvok zo zoznamu
	else{
		L->Last = L->Last->lptr;
		L->Last->rptr = NULL;
	}
	free(hlp);	
 
}

void DLPostDelete (tDLList *L)	{
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
//vytvorim si pomocnu premennu hlp(help)
	tDLElemPtr hlp;
	if(L->Act != NULL){
//zrusenie prvku za aktyvnym prvkom
		if(L->Act->rptr != NULL){
			hlp = L->Act->rptr;
			L->Act->rptr = hlp->rptr;
			if(hlp == L->Last)
				L->Last = L->Act;
//ak je aktivny prvok posledny nic sa nedeje
			else
				hlp->rptr->lptr = L->Act;
		}	
	}
}



void DLPreDelete (tDLList *L)	{
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
//vytvorim si pomocnu premennu hlp(help)
	tDLElemPtr hlp;
	if(L->Act != NULL){
//zrusenie prvku pred aktyvnym prvkom
		if(L->Act->lptr != NULL){
			hlp = L->Act->lptr;
			L->Act->lptr = hlp->lptr;
			if(hlp == L->First)
				L->First = L->Act;
//ak je aktivny prvok prvy nic sa nedeje
			else
				hlp->lptr->rptr = L->Act;
		}	
	}	

}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
//ak je aktivny prvok aktualny nic sa nedeje
	if(L->Act == NULL)
		return;
//alokujem si novy prvok v pamati(resp miesto na neho)
	tDLElemPtr hlp = malloc(sizeof(struct tDLElem));
//skuska ci sa alokacia podarila
	if(hlp == NULL) {
		DLError();
		return;
	}
//pridelenie hodnot novemu prvku a vlozenie ho na pravy ukazatel aktivneho prvku	
	hlp->data = val;
	hlp->rptr = L->Act->rptr;
	hlp->lptr = L->Act;
	L->Act->rptr = hlp;
//ak je aktualny prvok posledny tak pomocny vlozim na posledny		
	if(L->Act == L->Last)
		L->Last = hlp;
		
	else
		hlp->rptr->lptr = hlp;
			
		
		

}

void DLPreInsert (tDLList *L, int val)		{
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
//ak je aktivny prvok aktualny nic sa nedeje
	if(L->Act == NULL)
		return;
//alokujem si novy prvok v pamati(resp miesto na neho)
	tDLElemPtr hlp = malloc(sizeof(struct tDLElem));
//skuska ci sa alokacia podarila
	if(hlp == NULL) {
		DLError();
		return;
	}
//pridelenie hodnot novemu prvku a vlozenie ho na pravy ukazatel aktivneho prvku
	hlp->data = val;
	hlp->lptr = L->Act->lptr;
	hlp->rptr = L->Act;
	L->Act->lptr = hlp;
//ak je aktualny prvok prvy tak pomocny vlozim na prvy		
	if(L->Act == L->First)
		L->First = hlp;
		
	else
		hlp->lptr->rptr = hlp;	
	
}

void DLCopy (tDLList *L, int *val)	{
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
//ak je zoznam neaktivny, vola sa error
	if(L->Act == NULL) {
		DLError();
		return;
	}
//ulozenie hodnoty aktivneho prvku do parametru val
	*val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
//zistenie ci je zoznam aktivny
	if(L->Act == NULL) 
		return;
//na obsah aktivneho prvku vlozi hodnotu val	
	L->Act->data = val;
	
 
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
//zisti ci je zoznam aktivny ak nieje nedela nic
	if(L->Act == NULL) 
		return;
//presunie aktivitu na dalsi prvok zoznamu L
	L->Act = L->Act->rptr;

}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
//zisti ci je zoznam aktivny ak nieje nedela nic
	if(L->Act == NULL) 
		return;
//presunie aktivitu na predchadzajuci prvok zoznamu L
	L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
//ak nieje zoznam aktivny vraciam false inak true
		return L->Act != NULL;
}

/* Konec c206.c*/
