
/* ******************************* c202.c *********************************** */
/*  P�edm�t: Algoritmy (IAL) - FIT VUT v Brn�                                 */
/*  �kol: c202 - Z�sobn�k znak� v poli                                        */
/*  Referen�n� implementace: Petr P�ikryl, 1994                               */
/*  Vytvo�il: V�clav Topinka, z��� 2005                                       */
/*  �pravy: Bohuslav K�ena, ��jen 2013                                        */
/* ************************************************************************** */
/*
** Implementujte datov� typ z�sobn�k znak� ve statick�m poli. ADT z�sobn�k je
** reprezentov�n z�znamem typu tStack. Statick� pole 'arr' je indexov�no
** do maxim�ln� hodnoty STACK_SIZE. Polo�ka z�znamu 'top' ukazuje na prvek
** na vrcholu z�sobn�ku. Pr�zdn�mu z�sobn�ku odpov�d� hodnota top == -1,
** z�sobn�ku s jedn�m prvkem hodnota 0, atd. P�esnou definici typ� neleznete
** v hlavi�kov�m souboru c202.h. 
**
** Implementujte n�sleduj�c� funkce:
**
**    stackInit .... inicializace z�sobn�ku
**    stackEmpty ... test na pr�zdnost z�sobn�ku
**    stackFull .... test na zapln�nost z�sobn�ku
**    stackTop ..... p�e�te hodnotu z vrcholu z�sobn�ku
**    stackPop ..... odstran� prvek z vrcholu z�sobn�ku
**    stackPush .... vlo�� prvku do z�sobn�ku
**
** Sv� �e�en� ��eln� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**
**/

#include "c202.h"

int STACK_SIZE = MAX_STACK;
int solved;

void stackError ( int error_code ){
/*   ----------
** Vytiskne upozorn�n�, �e do�lo k chyb� p�i pr�ci se z�sobn�kem.
**
** TUTO FUNKCI, PROS�ME, NEUPRAVUJTE!
*/
	static const char* SERR_STRINGS[MAX_SERR+1] = {"Unknown error","Stack error: INIT","Stack error: PUSH","Stack error: TOP"};
	if ( error_code <= 0 || error_code > MAX_SERR )
		error_code = 0;
	printf ( "%s\n", SERR_STRINGS[error_code] );
	err_flag = 1;
}

void stackInit ( tStack* s ) {
/*   ---------
** Provede inicializaci z�sobn�ku - nastav� vrchol z�sobn�ku.
** Hodnoty ve statick�m poli neupravujte - po inicializaci z�sobn�ku
** jsou nedefinovan�.
**
** V p��pad�, �e funkce dostane jako parametr s == NULL,
** volejte funkci stackError(SERR_INIT). U ostatn�ch funkc� pro zjednodu�en�
** p�edpokl�dejte, �e tato situace nenastane. 
*/
	if (s == NULL){
		stackError( SERR_INIT );
		return;
	}
//inicializuje zasobnik, na vrchol dam hodnotu -1 co odpoveda prazdnemu zasobniku	
	s->top=-1;
	
}

int stackEmpty ( const tStack* s ) {
/*  ----------
** Vrac� nenulovou hodnotu, pokud je z�sobn�k pr�zdn�, jinak vrac� hodnotu 0.
** Funkci implementujte jako jedin� p��kaz. Vyvarujte se zejm�na konstrukce
** typu "if ( true ) b=true else b=false".
*/
//testujem ci je zasobnik prazdny ak ano vraciam -1
	return s->top == -1;

}

int stackFull ( const tStack* s ) {
/*  ---------
** Vrac� nenulovou hodnotu, je-li z�sobn�k pln�, jinak vrac� hodnotu 0.
** Dejte si zde pozor na �astou program�torskou chybu "o jedni�ku"
** a dob�e se zamyslete, kdy je z�sobn�k pln�, jestli�e m��e obsahovat
** nejv��e STACK_SIZE prkv� a prvn� prvek je vlo�en na pozici 0.
**
** Funkci implementujte jako jedin� p��kaz.
*/
//porovnam ci zasobnik nieje plny(ak vrchol reprezentuje posledny mozny prvok -1 koli tomu ze c pocita od 0)
	return s->top == (STACK_SIZE - 1);
}

void stackTop ( const tStack* s, char* c ) {
/*   --------
** Vrac� znak z vrcholu z�sobn�ku prost�ednictv�m parametru c.
** Tato operace ale prvek z vrcholu z�sobn�ku neodstra�uje.
** Vol�n� operace Top p�i pr�zdn�m z�sobn�ku je nekorektn�
** a o�et�ete ho vol�n�m funkce stackError(SERR_TOP). 
**
** Pro ov��en�, zda je z�sobn�k pr�zdn�, pou�ijte d��ve definovanou
** funkci stackEmpty.
*/
//test praznosti zasobniku
	int test = stackEmpty(s);

	if(test!=0){
		stackError( STACK_SIZE );
		return;
	}
//ak nieje zasobnik prazdny do c ulozi hodnotu z vrcholu zasobniku
	else 
		*c = s->arr[s->top];
	
}


void stackPop ( tStack* s ) {
/*   --------
** Odstran� prvek z vrcholu z�sobn�ku. Pro ov��en�, zda je z�sobn�k pr�zdn�,
** pou�ijte d��ve definovanou funkci stackEmpty.
**
** Vyvol�n� operace Pop p�i pr�zdn�m z�sobn�ku je sice podez�el� a m��e
** signalizovat chybu v algoritmu, ve kter�m je z�sobn�k pou�it, ale funkci
** pro o�et�en� chyby zde nevolejte (m��eme z�sobn�k ponechat pr�zdn�).
** Sp�e ne� vol�n� chyby by se zde hodilo vypsat varov�n�, co� v�ak pro
** jednoduchost ned�l�me.
** 
*/
	int test = stackEmpty(s);
//zistenie ci je zasobnik prazdny ak ano skonci podla zadania
	if (test != 0)
		return;
//ak zasobnik nieje prazdny prevedie funkciu pop(odstrani prvok z vrcholu zasobniku)
	s->top = s->top-1;
}


void stackPush ( tStack* s, char c ) {
/*   ---------
** Vlo�� znak na vrchol z�sobn�ku. Pokus o vlo�en� prvku do pln�ho z�sobn�ku
** je nekorektn� a o�et�ete ho vol�n�m procedury stackError(SERR_PUSH).
**
** Pro ov��en�, zda je z�sobn�k pln�, pou�ijte d��ve definovanou
** funkci stackFull.
*/
	int SFull=stackFull(s);
//test plnosti
	if (SFull !=0){
		stackError( SERR_PUSH );
		return;
	}
//prevedie funkciu push(prida prvok na vrchol zasobniku tym, ze zvysi top o 1 a ulozi tam znak c
	else{
		s->top = s->top+1;
		s->arr[s->top] = c;
	}
}

/* Konec c202.c */
