	
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                             �pravy: Bohuslav K�ena, ��jen 2013
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu, 
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek, 
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku, 
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu, 
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem, 
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu, 
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam 
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/	
    printf ("*ERROR* This program has performed an illegal operation.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L)	{
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
//inicializujem si zoznam(nastavenim prveho aktualneho a posledneho prvnu na NULL)
    L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;

}

void DLDisposeList (tDLList *L)	{
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free. 
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
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
//nastavenie aktivity(act) na prvy prvok
	L->Act = L->First;
}

void DLLast (tDLList *L)	{
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
//nastavenie aktivity(act) na posledny prvok
	L->Act = L->Last;	
}

void DLCopyFirst (tDLList *L, int *val)	{
/*
** Prost�ednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
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
** Prost�ednictv�m parametru val vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
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
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita 
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
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
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
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
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
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
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
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
** Prost�ednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
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
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/
//zistenie ci je zoznam aktivny
	if(L->Act == NULL) 
		return;
//na obsah aktivneho prvku vlozi hodnotu val	
	L->Act->data = val;
	
 
}

void DLSucc (tDLList *L)	{
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/
//zisti ci je zoznam aktivny ak nieje nedela nic
	if(L->Act == NULL) 
		return;
//presunie aktivitu na dalsi prvok zoznamu L
	L->Act = L->Act->rptr;

}


void DLPred (tDLList *L)	{
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/
//zisti ci je zoznam aktivny ak nieje nedela nic
	if(L->Act == NULL) 
		return;
//presunie aktivitu na predchadzajuci prvok zoznamu L
	L->Act = L->Act->lptr;
}

int DLActive (tDLList *L) {		
/*
** Je-li seznam aktivn�, vrac� true. V opa�n�m p��pad� vrac� false.
** Funkci implementujte jako jedin� p��kaz.
**/
//ak nieje zoznam aktivny vraciam false inak true
		return L->Act != NULL;
}

/* Konec c206.c*/
