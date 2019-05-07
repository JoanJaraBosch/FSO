/*****************************************************************************/
/*									     */
/*				     cocos0.c				     */
/*									     */
/*  Programa inicial d'exemple per a les practiques 2 i 3 d'ISO.	     */
/*     Es tracta del joc del menjacocos: es dibuixa un laberint amb una      */
/*     serie de punts (cocos), els quals han de ser "tragats" pel menja-     */
/*     cocos. Aquest menjacocos es representara amb el caracter '0', i el    */
/*     moura l'usuari amb les tecles 'w' (adalt), 's' (abaix), 'd' (dreta)   */
/*     i 'a' (esquerra). Simultaniament hi haura un conjunt de fantasmes,    */
/*     representats per numeros de l'1 al 9, que intentaran capturar al      */
/*     menjacocos. En la primera versio del programa, nomes hi ha un fan-    */
/*     tasma.								     */
/*     Evidentment, es tracta de menjar tots els punts abans que algun fan-  */
/*     tasma atrapi al menjacocos.					     */
/*									     */
/*  Arguments del programa:						     */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil1 n_col fit_tauler creq				     */
/*		mc_f mc_c mc_d						     */
/*		f1_f f1_c f1_d						     */
/*									     */
/*     on 'n_fil1', 'n_col' son les dimensions del taulell de joc, mes una   */
/*     fila pels missatges de text a l'ultima linia. "fit_tauler" es el nom  */
/*     d'un fitxer de text que contindra el dibuix del laberint, amb num. de */
/*     files igual a 'n_fil1'-1 i num. de columnes igual a 'n_col'. Dins     */
/*     d'aquest fitxer, hi hauran caracter ASCCII que es representaran en    */
/*     pantalla tal qual, excepte el caracters iguals a 'creq', que es visua-*/
/*     litzaran invertits per representar la paret.			     */
/*     Els parametres 'mc_f', 'mc_c' indiquen la posicio inicial de fila i   */
/*     columna del menjacocos, aixi com la direccio inicial de moviment      */
/*     (0 -> amunt, 1-> esquerra, 2-> avall, 3-> dreta). Els parametres	     */
/*     'f1_f', 'f1_c' i 'f1_d' corresponen a la mateixa informacio per al    */
/*     fantasma 1. El programa verifica que la primera posicio del menja-    */
/*     cocos o del fantasma no coincideixi amb un bloc de paret del laberint.*/
/*									     */
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment del menjacocos i dels fantasmes (en ms);           */
/*     el valor per defecte d'aquest parametre es 100 (1 decima de segon).   */
/*									     */
/*  Compilar i executar:					  	     */
/*     El programa invoca les funcions definides a 'winsuport.h', les        */
/*     quals propo_campionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				     */
/*									     */
/*	   $ gcc cocos0.c winsuport.o -o cocos0 -lcurses		     */
/*	   $ ./cocos0 fit_param [retard]				     */
/*									     */
/*  Codis de retorn:						  	     */
/*     El programa retorna algun dels seguents codis al SO:		     */
/*	0  ==>  funcionament normal					     */
/*	1  ==>  numero d'arguments incorrecte 				     */
/*	2  ==>  fitxer de configuracio no accessible			     */
/*	3  ==>  dimensions del taulell incorrectes			     */
/*	4  ==>  parametres del menjacocos incorrectes			     */
/*	5  ==>  parametres d'algun fantasma incorrectes			     */
/*	6  ==>  no s'ha pogut crear el camp de joc			     */
/*	7  ==>  no s'ha pogut inicialitzar el joc			     */
/*****************************************************************************/



#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per getpid() */
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "memoria.h"
#include "semafor.h"
#include "missatge.h"
#include "winsuport2.h"		/* incloure definicions de funcions propies */



#define MIN_FIL 7		/* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MAX_PROC 10

				/* definir estructures d'informacio */
typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
	char a;				/* caracter anterior en pos. actual */
} objecte;

int n_threads;

int minuts=0, segons=0;
pid_t tpid[MAX_PROC];

intptr_t *map_fi1 , *map_fi2, *map_camp;
int *map_bustia;
int id_fi1,id_fi2, id_camp, id_bustia_mem;
objecte f1;

/* variables globals */
int n_fil1, n_col;		/* dimensions del camp de joc */
char tauler[70];		/* nom del fitxer amb el laberint de joc */
char c_req;			/* caracter de pared del laberint */

objecte mc;      		/* informacio del menjacocos */
			/* informacio del fantasma 1 */

int df[] = {-1, 0, 1, 0};	/* moviments de les 4 direccions possibles */
int dc[] = {0, -1, 0, 1};	/* dalt, esquerra, baix, dreta */

int cocos;			/* numero restant de cocos per menjar */
int retard;		/* valor del retard de moviment, en mil.lisegons */

int id_sem;



/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins d'un fitxer de text, el nom del qual es passa per referencia a  */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris al principi del programa).		*/
void carrega_parametres(const char *nom_fit)
{
  FILE *fit;

  fit = fopen(nom_fit,"rt");		/* intenta obrir fitxer */
  if (fit == NULL)
  {	fprintf(stderr,"No s'ha pogut obrir el fitxer \'%s\'\n",nom_fit);
  	exit(2);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %s %c\n",&n_fil1,&n_col,tauler,&c_req);
  else {
	fprintf(stderr,"Falten parametres al fitxer \'%s\'\n",nom_fit);
	fclose(fit);
	exit(2);
	}
  if ((n_fil1 < MIN_FIL) || (n_fil1 > MAX_FIL) ||
	(n_col < MIN_COL) || (n_col > MAX_COL))
  {
	fprintf(stderr,"Error: dimensions del camp de joc incorrectes:\n");
	fprintf(stderr,"\t%d =< n_fil1 (%d) =< %d\n",MIN_FIL,n_fil1,MAX_FIL);
	fprintf(stderr,"\t%d =< n_col (%d) =< %d\n",MIN_COL,n_col,MAX_COL);
	fclose(fit);
	exit(3);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %d\n",&mc.f,&mc.c,&mc.d);
  else {
	fprintf(stderr,"Falten parametres al fitxer \'%s\'\n",nom_fit);
	fclose(fit);
	exit(2);
	}
  if ((mc.f < 1) || (mc.f > n_fil1-3) ||
	(mc.c < 1) || (mc.c > n_col-2) ||
	(mc.d < 0) || (mc.d > 3))
  {
	fprintf(stderr,"Error: parametres menjacocos incorrectes:\n");
	fprintf(stderr,"\t1 =< mc.f (%d) =< n_fil1-3 (%d)\n",mc.f,(n_fil1-3));
	fprintf(stderr,"\t1 =< mc.c (%d) =< n_col-2 (%d)\n",mc.c,(n_col-2));
	fprintf(stderr,"\t0 =< mc.d (%d) =< 3\n",mc.d);
	fclose(fit);
	exit(4);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %d\n",&f1.f,&f1.c,&f1.d);
  else {
	fprintf(stderr,"Falten parametres al fitxer \'%s\'\n",nom_fit);
	fclose(fit);
	exit(2);
	}
  if ((f1.f < 1) || (f1.f > n_fil1-3) ||
	(f1.c < 1) || (f1.c > n_col-2) ||
	(f1.d < 0) || (f1.d > 3))
    {
	fprintf(stderr,"Error: parametres fantasma 1 incorrectes:\n");
	fprintf(stderr,"\t1 =< f1.f (%d) =< n_fil1-3 (%d)\n",f1.f,(n_fil1-3));
	fprintf(stderr,"\t1 =< f1.c (%d) =< n_col-2 (%d)\n",f1.c,(n_col-2));
	fprintf(stderr,"\t0 =< f1.d (%d) =< 3\n",f1.d);
	fclose(fit);
	exit(5);
    }
  fclose(fit);			/* fitxer carregat: tot OK! */
  printf("Joc del MenjaCocos\n\tTecles: \'%c\', \'%c\', \'%c\', \'%c\', RETURN-> sortir\n",
		TEC_AMUNT, TEC_AVALL, TEC_DRETA, TEC_ESQUER);
  printf("prem una tecla per continuar:\n");
  getchar();
}




/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
void inicialitza_joc(void)
{
  int r,i,j;
  char strin[12];

  r = win_carregatauler(tauler,n_fil1-1,n_col,c_req);
  if (r == 0)
  {
    mc.a = win_quincar(mc.f,mc.c);
    if (mc.a == c_req) r = -6;		/* error: menjacocos sobre pared */
    else
    {
       f1.a = win_quincar(f1.f,f1.c);
       if (f1.a == c_req) r = -7;	/* error: fantasma sobre pared */
       else
       {
	cocos = 0;
	segons=0;
	minuts=0;			/* compta el numero total de cocos */
	for (i=0; i<n_fil1-1; i++)
	  for (j=0; j<n_col; j++)
	    if (win_quincar(i,j)=='.') cocos++;

        win_escricar(mc.f,mc.c,'C',NO_INV);
	win_escricar(f1.f,f1.c,'0',NO_INV);

        if (mc.a == '.') cocos--;	/* menja primer coco */

	sprintf(strin,"Cocos: %d", cocos); win_escristr(strin);
       }
    }
  }
  if (r != 0)
  {	win_fi();
	fprintf(stderr,"Error: no s'ha pogut inicialitzar el joc:\n");
	switch (r)
	{ case -1: fprintf(stderr,"  nom de fitxer erroni\n"); break;
	  case -2: fprintf(stderr,"  numero de columnes d'alguna fila no coincideix amb l'amplada del tauler de joc\n"); break;
	  case -3: fprintf(stderr,"  numero de columnes del laberint incorrecte\n"); break;
	  case -4: fprintf(stderr,"  numero de files del laberint incorrecte\n"); break;
	  case -5: fprintf(stderr,"  finestra de camp de joc no oberta\n"); break;
	  case -6: fprintf(stderr,"  posicio inicial del menjacocos damunt la pared del laberint\n"); break;
	  case -7: fprintf(stderr,"  posicio inicial del fantasma damunt la pared del laberint\n"); break;
	}
	exit(7);
  }
}

/* funcio per moure el menjacocos una posicio, en funcio de la direccio de   */
/* moviment actual; retorna -1 si s'ha premut RETURN, 1 si s'ha menjat tots  */
/* els cocos, i 0 altrament */
void* mou_menjacocos(void * nulo)
{
  char strin[33];
  objecte seg;
  int tec;
	int segons=0, minuts=0, tics=0;


	while (!(*map_fi1) && !(*map_fi2)){
		waitS(id_sem);
	  tec = win_gettec();
		signalS(id_sem);
	  if (tec != 0)
	   switch (tec)		/* modificar direccio menjacocos segons tecla */
	   {
	    case TEC_AMUNT:	mc.d = 0; break;
	    case TEC_ESQUER:	mc.d = 1; break;
	    case TEC_AVALL:	mc.d = 2; break;
	    case TEC_DRETA:	mc.d = 3; break;
	    case TEC_RETURN:	(*map_fi1) = -1; break;
	   }
	  seg.f = mc.f + df[mc.d];	/* calcular seguent posicio */
	  seg.c = mc.c + dc[mc.d];
		waitS(id_sem);
	  seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
	  if ((seg.a == ' ') || (seg.a == '.'))
	  {
	    win_escricar(mc.f,mc.c,' ',NO_INV);		/* esborra posicio anterior */
	    mc.f = seg.f; mc.c = seg.c;			/* actualitza posicio */
	    win_escricar(mc.f,mc.c,'C',NO_INV);		/* redibuixa menjacocos */
	    if (seg.a == '.')
	    {
		cocos--;
		if (cocos == 0) (*map_fi1) = 1;
	    }
	  }
		signalS(id_sem);
		sprintf(strin,"Cocos: %d    Minuts: %d Segons: %d", cocos, minuts, segons);

		waitS(id_sem);
		win_escristr(strin);
		signalS(id_sem);
		win_retard(retard);
		tics++;
		if(tics==(int)(1000/retard)){
			tics=0;
			segons++;
			if(segons==60){
				segons=0;
				minuts++;
			}
		}

	}
	pthread_exit((void *) 0);
}

/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
  int mida_camp;		/* variables locals */
  srand(getpid());		/* inicialitza numeros aleatoris */
	long int t_ini, t_fin;
	long int secs;
	char a1[20], a2[20], a3[20], a4[20], a5[20], a6[20], a7[20], a8[20], a9[20], a10[20], a11[20], a12[20], a13[20];

  if ((n_args != 3) && (n_args !=4))
  {	fprintf(stderr,"Comanda: cocos1 fit_param num_fantasmes [retard]\n");
  	exit(1);
  }
  carrega_parametres(ll_args[1]);
  n_threads = atoi(ll_args[2]);
	if(n_threads>MAX_PROC) n_threads=MAX_PROC;
	else if(n_threads<1)n_threads=1;

  if (n_args == 4) retard = atoi(ll_args[3]);
  else retard = 100;

  mida_camp = win_ini(&n_fil1,&n_col,'+',INVERS);	/* intenta crear taulell */
  if (mida_camp >= 0)		/* si aconsegueix accedir a l'entorn CURSES */
  {
		id_camp = ini_mem(mida_camp);
		map_camp = map_mem(id_camp);

		id_fi1 = ini_mem(sizeof(int));
		map_fi1 = map_mem(id_fi1);
		id_fi2 = ini_mem(sizeof(int));
		map_fi2 = map_mem(id_fi2);
		id_sem=ini_sem(1);
		id_bustia_mem = ini_mem(sizeof(int)*MAX_PROC);
		map_bustia = map_mem(id_bustia_mem);

		for(int k =0 ; k<MAX_PROC;k++){
			map_bustia[k]=ini_mis();
		}

		*map_fi1 = 0;
		*map_fi2 = 0;
		sprintf(a1,"%i",id_camp);
		sprintf(a2,"%i",id_fi1);
		sprintf(a3,"%i",id_fi2);
		sprintf(a8,"%i",n_fil1);
		sprintf(a9,"%i",n_col);
		sprintf(a10,"%i",retard);
		sprintf(a12,"%i",id_sem);
		sprintf(a13,"%i",id_bustia_mem);

		win_set(map_camp,n_fil1,n_col);
		inicialitza_joc();

		sprintf(a4,"%c",f1.a);
		sprintf(a5,"%i",f1.d);
		sprintf(a6,"%i",f1.f);
		sprintf(a7,"%i",f1.c);

    int i, n=0;
		time(&t_ini);
		for(i=0;i<n_threads;i++){
			tpid[n] = fork(); /* crea un nou proces */
			if (tpid[n] == (pid_t) 0) /* branca del fill */
			{
				sprintf(a11,"%i",i);
				execlp("./fantasma4", "fantasma4", a1, a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13, (char *)0);
	 			fprintf(stderr,"error: no puc executar el process fill \'fantasma3\'\n");
	 			exit(0);
			}else if (tpid[n] > 0) n++;
		}

	pthread_t coco;
	pthread_create(&coco, NULL,mou_menjacocos, NULL);

while(! (*map_fi1) && ! (*map_fi2)){
	win_update();
	win_retard(100);
}

	for (i = 0; i < n; i++)
	{
		waitpid(tpid[i],NULL,0);
	}

		pthread_join(coco,NULL);
    win_fi();
		time(&t_fin);
		if ((*map_fi1) == -1) printf("S'ha aturat el joc amb tecla RETURN!\n");
		else { if (*map_fi1) printf("Ha guanyat l'usuari!\n");
			 else printf("Ha guanyat l'ordinador!\n");}

			 secs = (t_fin-t_ini);
			 long int min = (int)(secs/60);
			 secs = ((int)secs%60);
		 printf("Ha tardat %ld minuts i %ld segons\n", min, secs);
		 if(cocos!=0){
			printf("Falten %d cocos per menjar\n", cocos);
		}
  }
  else
  {	fprintf(stderr,"Error: no s'ha pogut crear el taulell:\n");
	switch (mida_camp)
	{ case -1: fprintf(stderr,"camp de joc ja creat!\n");
		  break;
	  case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
		  break;
	  case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
		  break;
	  case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
		  break;
	}
	exit(6);
  }
	elim_sem(id_sem);
	elim_mem(id_camp);
	elim_mem(id_fi1);
	elim_mem(id_fi2);
	elim_mem(id_bustia_mem);

  return(0);
}
