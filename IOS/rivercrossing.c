/*
 * Soubor:  xondri04.c
 * Datum:   2014/5/3
 * Autor:   Ondris Matus, xondri04@stud.fit.vutbr.cz
 * Projekt: River crossing problem
 * Popis:   Riesi problem semaforov.*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

//Pomenovanie Semaforov
enum {
	SEM_SHRMEM = 0, //semafor sluziaci na pristu zapis vypis a menenie zdielanej pamate
	SEM_MOLO ,      //semafor sluziaci na nastup na molo
	SEM_NALOD ,     //semafor sluziaci na nalodenie 
	SEM_ROLA ,      //semafor sluziaci na vypis role (captain alebo member)
	SEM_PLAVBA ,    //semafor sluziaci na zacatie plavby
	SEM_VILOD ,     //semafor sluziaci na vilodenie a povoluje vypis landing
	SEM_UKONCI ,    //semafor sluziaci na ukoncenie povoluje vypis finished  
	SEM_NALODH ,    //semafor sluziaci na povolenie nalodenia hackera(zaistuje ze sa nalodi vzdy potrebny pocet hackers)
	SEM_NALODS ,    //semafor sluziaci na povolenie nalodenia serfa(zaistuje ze sa nalodi vzdy potrebny pocet serfs)
	SEM_CNT,	//pocitac semaforov (kolko ich je kedze ich pomenovanie riesim cez pole)
};

//Pole obsahujuce pociatocne hodnoty semaforov v poradi va akom su uvedene hore (values)
static unsigned sem_vals[SEM_CNT] = {
	1,	//semafor na pristup do zdielanej pamate je defaultne otvoreny
	1,	//semafor sluziaci na vstup na molo je defaultne otvoreny
	0,	//ostatne semafory su uzavrete
	0,
	0,
	0,
	0,
	0,
	0,
};

/** Kody chyb programu */
enum tecodes
{
	EOK = 0,     /**< Bez chyby */
	ECLWRONG,    /**< Chybny prikazovy radek. */
	EALOC,       /**< Chyba alokacie */
	EPROC,       /**< Chyba procesu */
	EUNKNOWN,    /**< Neznama chyba */
};

/** Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] =
{
	[EOK] = "Vsetko v poriadku.\n",
	[ECLWRONG] = "Chybne parametry prikazoveho radku!\n",
	[EALOC] = "Chyba alokacie zdialanej pamete!!!\n",
	[EPROC] = "Pri behu potomka doslo k chybe!!!\n",
	[EUNKNOWN] = "Nastala nepredvidana chyba! Vypnite pocitac a rychle utecte.\n",
};

/**
 * Struktura obsahujuca hodnoty parametrov prikazoveho riadku.
 */
typedef struct params
{
  int ecode;       /**< Chybovy kod programu, odpovida vyctu tecodes. */
  int P;           /**< P je počet osob generovaných v každé kategorii; bude vytvořeno P hackers a P serfs. P > 0 && P % 2 = 0. */
  int H;           /**< H je maximální hodnota doby (v milisekundách), po které je generován nový proces hacker. H >= 0 && H < 5001 */
  int S;           /**< S je maximální hodnota doby (v milisekundách), po které je generován nový proces serf. S >= 0 && S < 5001 */
  int R;           /**<  R je maximální hodnota doby (v milisekundách) plavby. R >= 0 && R < 5001 */
} TParams;

//struktura zdielanej pamate
struct shared {
	sem_t sems[SEM_CNT];  //pole semaforov pouzivam nepomenovane semafory , musia byt v zdielanej pamati
	int   citac;          //pocitadlo A (cislo vypisu)
	int   NH;             //NH Hakerov na mole
	int   NS;             //NS Serfs na mole
	int   nal;            //pomocna premenna nal sluzi ako citac (zaistuje ze dalsi vypis captain alebo member sa vypise az po nalodeni)
	int   vyp;            //pomocna premenna vyp sluzi ako citac (az ked vsetci vypisu moze zacat plavba a vypis landing)
	int   vil;            //pomocna premenna vil sluzi ako citac (az ked sa vsetci vilodia otvori sa opatovne molo)
};

struct shared *shared;
int            shared_id;
FILE          *fd;
//prototipy funkcii
void *init_shmem(size_t size, const char *path, int *id);
void destroy_shmem(void *ptr, int id);
void cleanup_sems(struct shared *s, int cnt);
int init_sems(struct shared *s);
void cleanup(void);

//---------------------------------------------funkcia serf ---------------------------------

void serf(int id, int S, int R, int P);

void serf(int id, int S, int R, int P) {
	usleep(rand()%1001*S);
	sem_wait(&shared->sems[SEM_SHRMEM]);
	shared->citac+= 1;
	fprintf(fd,"%d: serf: %d: started \n",shared->citac,id);
	sem_post(&shared->sems[SEM_SHRMEM]);
	sem_wait(&shared->sems[SEM_MOLO]);
	sem_wait(&shared->sems[SEM_SHRMEM]);
	shared->citac+= 1;
	shared->NS+= 1;
	fprintf(fd,"%d: serf: %d: waiting for boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

///////Zistenie ci sa jedna o kapitana
	if(((shared->NS)==4) || (((shared->NS)==2) && ((shared->NH)>=2))){
//osetrenie vsetkych moznych skupin ktore mozu v tomto pripade nastat
		if(((shared->NS)==4) && ((shared->NH)==1)){	
			(shared->NS) = 0;
			(shared->NH) = 1;
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
		}
		else if(((shared->NS)==4) && ((shared->NH)==0)){	
			(shared->NS) = 0;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
		}
		else if(((shared->NS)==2) && ((shared->NH)==3)){	
			(shared->NS) = 0;
			(shared->NH) = 1;
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODS]);
		}
		else if(((shared->NS)==2) && ((shared->NH)==2)){	
			(shared->NS) = 0;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODS]);
		}
		sem_post(&shared->sems[SEM_NALOD]);
		sem_post(&shared->sems[SEM_SHRMEM]);
		sem_wait(&shared->sems[SEM_NALOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);
		shared->citac+= 1;
		shared->nal+= 1;
		fprintf(fd,"%d: serf: %d: boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->nal)<4){
			sem_post(&shared->sems[SEM_NALOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->nal = 0;
			sem_post(&shared->sems[SEM_ROLA]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		sem_wait(&shared->sems[SEM_ROLA]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vyp+= 1;
		fprintf(fd,"%d: serf: %d: captain \n",shared->citac,id);

		if((shared->vyp)<4){
			sem_post(&shared->sems[SEM_ROLA]);
			sem_post(&shared->sems[SEM_SHRMEM]);
			sem_wait(&shared->sems[SEM_PLAVBA]); //caka kym vsetci vypisu capt or member a simuluje plavbu
			usleep(rand()%1001*R);
			sem_post(&shared->sems[SEM_VILOD]);
		}
		else{
			shared->vyp = 0;
			usleep(rand()%1001*R);
			sem_post(&shared->sems[SEM_VILOD]); //ak je posledny co vypisal simuluje plavbu
			sem_post(&shared->sems[SEM_SHRMEM]);
		}	
		sem_wait(&shared->sems[SEM_VILOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vil+= 1;
		fprintf(fd,"%d: serf: %d: landing: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->vil)<4){
			sem_post(&shared->sems[SEM_VILOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->vil = 0;
			sem_post(&shared->sems[SEM_MOLO]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
	}
	else{   ////Stava sa member a cini ukony membra
		sem_post(&shared->sems[SEM_SHRMEM]);
		sem_post(&shared->sems[SEM_MOLO]);	
//zistuje ci je pre neho volne miesto na lodi
		sem_wait(&shared->sems[SEM_NALODS]);
		sem_wait(&shared->sems[SEM_NALOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);
		shared->citac+= 1;
		shared->nal+= 1;
		fprintf(fd,"%d: serf: %d: boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->nal)<4){
			sem_post(&shared->sems[SEM_NALOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->nal = 0;
			sem_post(&shared->sems[SEM_ROLA]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		sem_wait(&shared->sems[SEM_ROLA]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vyp+= 1;
		fprintf(fd,"%d: serf: %d: member \n",shared->citac,id);

		if((shared->vyp)<4){
			sem_post(&shared->sems[SEM_ROLA]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->vyp = 0;
			sem_post(&shared->sems[SEM_PLAVBA]); //ak je posledny posiela kapitanovi info ze moze zacat palvbu
			sem_post(&shared->sems[SEM_SHRMEM]);
		}	
		sem_wait(&shared->sems[SEM_VILOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vil+= 1;
		fprintf(fd,"%d: serf: %d: landing: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->vil)<4){
			sem_post(&shared->sems[SEM_SHRMEM]);
			sem_post(&shared->sems[SEM_VILOD]);
		}
		else{
			shared->vil = 0;
			sem_post(&shared->sems[SEM_MOLO]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		
	}
/*
caka na vylodenie posledneho clena poslednej skupiny vymyslel som si na to vzorec pomocou ktoreho to overi , nech tu je aj trocha matiky
Zisti ci vypis landing ja uz posledny pomocou vzorca pocet osob*2 (pre vsetkych h aj s) * 5(pocet vypisov ktore musi kazda osoba vypisat
od started az po finished a porovna to z aktualnym vypisom , ci nieje posledny
*/
	if((shared->citac) == ((P*2)*5)){
		sem_post(&shared->sems[SEM_UKONCI]);
	}

	sem_wait(&shared->sems[SEM_UKONCI]);
	sem_wait(&shared->sems[SEM_SHRMEM]);	
	shared->citac+= 1;
	fprintf(fd,"%d: serf: %d: finished \n",shared->citac,id); 
	sem_post(&shared->sems[SEM_UKONCI]);
	sem_post(&shared->sems[SEM_SHRMEM]);	
	return;
}
//----------------------------------------funkcia hackera--------------------------------------

void hacker(int id, int H, int R, int P);

void hacker(int id, int H, int R, int P) {
	usleep(rand()%1001*H);
	sem_wait(&shared->sems[SEM_SHRMEM]);
	shared->citac+= 1;
	fprintf(fd,"%d: hacker: %d: started \n",shared->citac,id);
	sem_post(&shared->sems[SEM_SHRMEM]);
	sem_wait(&shared->sems[SEM_MOLO]);
	sem_wait(&shared->sems[SEM_SHRMEM]);
	shared->citac+= 1;
	shared->NH+= 1;
	fprintf(fd,"%d: hacker: %d: waiting for boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);
///////Zistenie ci sa jedna o kapitana
	if(((shared->NH)==4) || (((shared->NH)==2) && ((shared->NS)>=2))){
//osetrenie vsetkych stavov skupin ktore mozu nastat
		if(((shared->NH)==4) && ((shared->NS)==1)){	
			(shared->NS) = 1;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
		}
		else if(((shared->NH)==4) && ((shared->NS)==0)){	
			(shared->NS) = 0;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODH]);
		}
		else if(((shared->NH)==2) && ((shared->NS)==3)){	
			(shared->NS) = 1;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODH]);
		}
		else if(((shared->NH)==2) && ((shared->NS)==2)){	
			(shared->NS) = 0;
			(shared->NH) = 0;
			sem_post(&shared->sems[SEM_NALODH]);
			sem_post(&shared->sems[SEM_NALODS]);
			sem_post(&shared->sems[SEM_NALODS]);
		}
		sem_post(&shared->sems[SEM_NALOD]);
		sem_post(&shared->sems[SEM_SHRMEM]);
		sem_wait(&shared->sems[SEM_NALOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);
		shared->citac+= 1;
		shared->nal+= 1;
		fprintf(fd,"%d: hacker: %d: boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);
		if((shared->nal)<4){
			sem_post(&shared->sems[SEM_NALOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->nal = 0;
			sem_post(&shared->sems[SEM_ROLA]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		sem_wait(&shared->sems[SEM_ROLA]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vyp+= 1;
		fprintf(fd,"%d: hacker: %d: captain \n",shared->citac,id);

		if((shared->vyp)<4){
			sem_post(&shared->sems[SEM_ROLA]);
			sem_post(&shared->sems[SEM_SHRMEM]);
			sem_wait(&shared->sems[SEM_PLAVBA]);
			usleep(rand()%1001*R);
			sem_post(&shared->sems[SEM_VILOD]);
		}
		else{
			shared->vyp = 0;
			usleep(rand()%1001*R);
			sem_post(&shared->sems[SEM_VILOD]); //ak je posledny co vypisal simuluje plavbu
			sem_post(&shared->sems[SEM_SHRMEM]);
		}	
		sem_wait(&shared->sems[SEM_VILOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vil+= 1;
		fprintf(fd,"%d: hacker: %d: landing: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->vil)<4){
			sem_post(&shared->sems[SEM_VILOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->vil = 0;
			sem_post(&shared->sems[SEM_MOLO]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}

	}
	else{   ////Stava sa member a cini ukony membra
		sem_post(&shared->sems[SEM_SHRMEM]);
		sem_post(&shared->sems[SEM_MOLO]);	
		sem_wait(&shared->sems[SEM_NALODH]);
		sem_wait(&shared->sems[SEM_NALOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);
		shared->citac+= 1;
		shared->nal+= 1;
		fprintf(fd,"%d: hacker: %d: boarding: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->nal)<4){
			sem_post(&shared->sems[SEM_NALOD]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->nal = 0;
			sem_post(&shared->sems[SEM_ROLA]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		sem_wait(&shared->sems[SEM_ROLA]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vyp+= 1;
		fprintf(fd,"%d: hacker: %d: member \n",shared->citac,id);

		if((shared->vyp)<4){
			sem_post(&shared->sems[SEM_ROLA]);
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		else{
			shared->vyp = 0;
			sem_post(&shared->sems[SEM_PLAVBA]); //ak je posledny posiela kapitanovi info ze moze zacat palvbu
			sem_post(&shared->sems[SEM_SHRMEM]);
		}	
		sem_wait(&shared->sems[SEM_VILOD]);
		sem_wait(&shared->sems[SEM_SHRMEM]);	
		shared->citac+= 1;
		shared->vil+= 1;
		fprintf(fd,"%d: hacker: %d: landing: %d: %d \n",shared->citac,id,shared->NH,shared->NS);

		if((shared->vil)<4){
			sem_post(&shared->sems[SEM_SHRMEM]);
			sem_post(&shared->sems[SEM_VILOD]);
		}
		else{
			shared->vil = 0;
			sem_post(&shared->sems[SEM_MOLO]); //ak je posledny co sa nalodil povoluje vypis capt a member
			sem_post(&shared->sems[SEM_SHRMEM]);
		}
		
	}

	if((shared->citac) == ((P*2)*5)){
		sem_post(&shared->sems[SEM_UKONCI]);
	}

	sem_wait(&shared->sems[SEM_UKONCI]);
	sem_wait(&shared->sems[SEM_SHRMEM]);	
	shared->citac+= 1;
	fprintf(fd,"%d: hacker: %d: finished \n",shared->citac,id); 
	sem_post(&shared->sems[SEM_UKONCI]);
	sem_post(&shared->sems[SEM_SHRMEM]);	
	
	return;

}

//------------------------------------------------------------------------------

void *init_shmem(size_t size, const char *path, int *id) {
	key_t key = ftok(path, 5);
	if(key == -1) {
		return NULL;
	}

	if((*id = shmget(key, size, IPC_CREAT | 0660)) == -1) {
		return NULL;
	}

	void *ret;
	if((ret = shmat(*id, NULL, 0)) == (void*)-1) {
		return NULL;
	}
	return ret;
}

//----------------------------funkcia sluziaca na odstranenie zdielanej pamate--------------------------------------------------

void destroy_shmem(void *ptr, int id) {
	shmdt(ptr);
	shmctl(id, IPC_RMID, NULL);
}

//----------------------------funkcia sluziaca an odstranenie semaforov--------------------------------------------------

void cleanup_sems(struct shared *s, int cnt) {
	int pos;
	for(pos = 0; pos < cnt; pos++) {
		sem_destroy(&s->sems[pos]);
	}
}

//----------------------------funkcia sluziaca na inicializaciu semaforov--------------------------------------------------

int init_sems(struct shared *s) {
	int pos;
	for(pos = 0; pos < SEM_CNT; pos++) {
		if(sem_init(&s->sems[pos], 1, sem_vals[pos]) == -1) {
			cleanup_sems(s, pos);
			return -1;
		}
	}
	return 0;
}

//-----------------------------spracovanie parametrov-------------------------------------------------
TParams getParams(int argc, char *argv[])
{
	char *stop = NULL;

	TParams result =
	{ 
		.ecode = EOK,
	};

	if(argc == 5) {
	// postupne overime jednotlive parametry
		if(isdigit(*argv[1])) {
			result.P = strtoul(argv[1], &stop, 10);
		}
		else {
			result.ecode = ECLWRONG;
		}
		if(isdigit(*argv[2])) {
			result.H = strtoul(argv[2], &stop, 10);
		} 
		else {
			result.ecode = ECLWRONG;
		}
		if(isdigit(*argv[3])) {
			result.S = strtoul(argv[3], &stop, 10);
		}
		else {
			result.ecode = ECLWRONG;
		}
		if(isdigit(*argv[4])) {
			result.R = strtoul(argv[4], &stop, 10);
		}
		else {
		result.ecode = ECLWRONG;
		}
	} 
	else {
		result.ecode = ECLWRONG;
	}
	if((result.P <= 0) || ((result.P %2 ) != 0) || (result.H < 0) || (result.H >= 5001) || (result.S < 0) || (result.S >= 5001) || (result.R < 0) || (result.R >= 5001)){
		result.ecode = ECLWRONG;
	}
	return result;
}

//---------------------------chybove vypisi---------------------------------------------------

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu.
 * @param ecode kod chyby programu
 */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EUNKNOWN)
		{ ecode = EUNKNOWN; }

	fprintf(stderr, "%s", ECODEMSG[ecode]);
}


//----------------------uvilnenie zdrojov--------------------------------------------------------

void cleanup(void) {
	cleanup_sems(shared, SEM_CNT);
	destroy_shmem(shared, shared_id);
	fclose(fd);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {

	TParams params = getParams(argc, argv);

	if (params.ecode != EOK)
	{
		printECode(params.ecode);
		return 1;
	}
	fd = fopen("rivercrossing.out","w");
	if (fd == NULL)
	{
		perror("fopen()");
		return 2;
	}
	else
		setbuf(fd,NULL);

	shared = init_shmem(sizeof(struct shared), argv[0], &shared_id);
	if(shared == NULL) {
		fclose(fd);
		return 2;
	}

	if(init_sems(shared) == -1) {
		destroy_shmem(shared, shared_id);
		fclose(fd);
		return 2;
	}

	if(atexit(cleanup) != 0) {
		cleanup();
		return 2;
	}
	//	inicializacia poloziek zdielanej pamate
	shared->citac = 0;
	shared->NH = 0;
	shared->NS = 0;
	shared->nal = 0;
	shared->vyp = 0;
	shared->vil = 0;
	// forkovanie a pustanie procesov osob + testovanie uspesnosti
	unsigned chd_cnt = 0;
	pid_t pid;
	int i;
	for(i = 0; i<params.P; i++){
		pid = fork();
		if(pid == -1){
			perror("fork()");
			break;
		}
		if(pid == 0){
			serf(i+1, params.S, params.R, params.P);
			exit(0);
		}
		chd_cnt++;
		pid = fork();
		if(pid == -1){
			perror("fork()");
			break;
		}
		if(pid == 0){
			hacker(i+1, params.H, params.R, params.P);
			exit(0);
		}
		chd_cnt++;
	}

	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	if(i != params.P) {
		kill(0, SIGINT);
	}

	while(chd_cnt != 0){
		waitpid(-1, NULL, 0);
		chd_cnt--;
	}

	return params.P == i ? 0 : 2;
}
