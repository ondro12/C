
/* ******************************* c204.c *********************************** */
/*  Pøedmìt: Algoritmy (IAL) - FIT VUT v Brnì                                 */
/*  Úkol: c204 - Pøevod infixového výrazu na postfixový (s vyu¾itím c202)     */
/*  Referenèní implementace: Petr Pøikryl, listopad 1994                      */
/*  Pøepis do jazyka C: Luká¹ Mar¹ík, prosinec 2012                           */
/*  Upravil: Bohuslav Køena, øíjen 2013                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro pøevod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro pøevod vyu¾ijte zásobník (tStack),
** který byl implementován v rámci pøíkladu c202. Bez správného vyøe¹ení
** pøíkladu c202 se o øe¹ení tohoto pøíkladu nepokou¹ejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro pøevod infixového výrazu 
**                       na postfixový
**
** Pro lep¹í pøehlednost kódu implementujte následující funkce:
**    
**    untilLeftPar .... vyprázdnìní zásobníku a¾ po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své øe¹ení úèelnì komentujte.
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slou¾í k vyprázdnìní zásobníku a¾ po levou závorku, pøièem¾ levá závorka
** bude také odstranìna. Pokud je zásobník prázdný, provádìní funkce se ukonèí.
**
** Operátory odstraòované ze zásobníku postupnì vkládejte do výstupního pole
** znakù postExpr. Délka pøevedeného výrazu a té¾ ukazatel na první volné
** místo, na které se má zapisovat, pøedstavuje parametr postLen.
**
** Aby se minimalizoval poèet pøístupù ke struktuøe zásobníku, mù¾ete zde
** nadeklarovat a pou¾ívat pomocnou promìnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {
//kontrola ci je zasobnik prazdny
	if (stackEmpty(s) != 0)
		return;
//citanie + zapisovanie zo zasobnika do vystupneho pola po nacitani prevedie funkciu pop(cita az po najblizsiu lavu zatvorku)
	while(s->arr[s->top] != '('){
		postExpr[*postLen] = s->arr[s->top];
		stackPop(s);
		(*postLen)++;
	}
	if(s->arr[s->top] == '(')
		stackPop(s);
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je pøedán parametrem c po naètení znaku ze
** vstupního pole znakù.
**
** Dle priority pøedaného operátoru a pøípadnì priority operátoru na
** vrcholu zásobníku rozhodneme o dal¹ím postupu. Délka pøevedeného 
** výrazu a takté¾ ukazatel na první volné místo, do kterého se má zapisovat, 
** pøedstavuje parametr postLen, výstupním polem znakù je opìt postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {
//kontrola ci nieje zasobnik prazdny, ak nieje prevedie danu funkciu
	if(stackEmpty(s) != 0)
//spracovavany operator vlozi na vrchol zasobniku
		stackPush(s,c);

//ak je na vrchole zasobniku lava zatvorka vlozi dany operator na vrchol zasobniku
	else if(s->arr[s->top] == '(')
		stackPush(s,c);
//kontrola ci na vrchole zasobniku je prvok s rovnakou prioritou(pre znamienka + a -)ak je prvok nacita do vystupneho pola a odstrani z vrcholu stary prvok a vlozi novy prvok
	else if(c == '+' || c == '-'){
		postExpr[*postLen] = s->arr[s->top];
		stackPop(s);
		stackPush(s,c);
		(*postLen) ++;
	}
//kontrola ci na vrchole zasobniku je prvok s rovnakou prioritou(pre znamienka * a /)ak je prvok nacita do vystupneho pola a odstrani z vrcholu stary prvok a vlozi novy prvok
	else if(c == '*' || c == '/'){
		if(s->arr[s->top] == '+' || s->arr[s->top] == '-' || s->arr[s->top] == '*' || s->arr[s->top] == '/'){
		postExpr[*postLen] = s->arr[s->top];
		(*postLen) ++;
		stackPop(s);
		stackPush(s,c);
		}
	}
}

/* 
** Konverzní funkce infix2postfix.
** Ète infixový výraz ze vstupního øetìzce infExpr a generuje
** odpovídající postfixový výraz do výstupního øetìzce (postup pøevodu
** hledejte v pøedná¹kách nebo ve studijní opoøe). Pamì» pro výstupní øetìzec
** (o velikosti MAX_LEN) je tøeba alokovat. Volající funkce, tedy
** pøíjemce konvertovaného øetìzce, zajistí korektní uvolnìní zde alokované
** pamìti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sèítání, odèítání,
**    násobení a dìlení. Sèítání má stejnou prioritu jako odèítání,
**    násobení má stejnou prioritu jako dìlení. Priorita násobení je
**    vìt¹í ne¾ priorita sèítání. V¹echny operátory jsou binární
**    (neuva¾ujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a èíslicemi - 0..9, a..z, A..Z (velikost písmen se rozli¹uje).
**
** 3. Ve výrazu mù¾e být pou¾it pøedem neurèený poèet dvojic kulatých
**    závorek. Uva¾ujte, ¾e vstupní výraz je zapsán správnì (neo¹etøujte
**    chybné zadání výrazu).
**
** 4. Ka¾dý korektnì zapsaný výraz (infixový i postfixový) musí být uzavøen 
**    ukonèovacím znakem '='.
**
** 5. Pøi stejné prioritì operátorù se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník pou¾ijte zásobník znakù tStack implementovaný v pøíkladu c202. 
** Pro práci se zásobníkem pak pou¾ívejte výhradnì operace z jeho rozhraní.
**
** Pøi implementaci vyu¾ijte pomocné funkce untilLeftPar a doOperation.
**
** Øetìzcem (infixového a postfixového výrazu) je zde my¹leno pole znakù typu
** char, jen¾ je korektnì ukonèeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu oèekávejte pouze korektnì zapsané a ukonèené výrazy. Jejich délka
** nepøesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se mìl vejít do alokovaného pole. Po alokaci dynamické pamìti si v¾dycky
** ovìøte, ¾e se alokace skuteènì zdraøila. V pøípadì chyby alokace vra»te namísto
** øetìzce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {
//alokacia pamate pre vystupne pole
	char* postExpr = malloc(sizeof(char)*MAX_LEN);
	if(postExpr == NULL) {
		return NULL;
	}
//inicializacia zasobniku
	tStack s;
	stackInit (&s);	
	int i = 0;
	unsigned postLen = 0;	
//nacitavanie zo vstupneho pola az po znamianko =
	while(infExpr[i] != '='){
		if(infExpr[i] == '(')
			stackPush (&s,infExpr[i]);
//ak nacitavany prvok je operator ak ano vola dopredu nadeklarovanu funkciu
		else if(infExpr[i] == '+' || infExpr[i] == '-' || infExpr[i] == '*' || infExpr[i] == '/')
			doOperation(&s,infExpr[i],postExpr,&postLen);
//ak je spracovavany znak prava zatvorka vola funkciu postlen ktora cita az po lavu zatvorku zo zasobnika a posiela na vystup
		else if(infExpr[i] == ')')	
			untilLeftPar(&s,postExpr,&postLen);
		else{
			postExpr[postLen] = infExpr[i];
			postLen++;
		}	
	i++;
	}
	while(!stackEmpty(&s)){
		stackTop (&s, &postExpr[postLen]);
		stackPop(&s);
		postLen++;
	}
	postExpr[postLen] = '=';


	return postExpr;
	free(postExpr);

}

/* Konec c204.c */
