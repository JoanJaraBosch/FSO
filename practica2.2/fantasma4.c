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

typedef struct {		/* per un objecte (menjacocos o fantasma) */
	int f;				/* posicio actual: fila */
	int c;				/* posicio actual: columna */
	int d;				/* direccio actual: [0..3] */
	char a;				/* caracter anterior en pos. actual */
} objecte;

int main(int n_args, const char *ll_args[])
{
	intptr_t *map_fi1 , *map_fi2, *map_camp;
	int midaCamp, id_sem,id_bustia, fi1, fi2, nfil, ncol, retard, index;
  int k, vk, nd, vd[3];
	objecte seg, actual;
	int df[] = {-1,0,1,0};
	int dc[] = {0,-1,0,1};

	actual.a=ll_args[4][0];
	actual.c=atoi(ll_args[7]);
	actual.f=atoi(ll_args[6]);
	actual.d=atoi(ll_args[5]);

	retard=atoi(ll_args[10]);
	index=atoi(ll_args[11]);
	id_sem = atoi(ll_args[12]);
	fi1 = atoi(ll_args[2]);
	fi2 = atoi(ll_args[3]);
	midaCamp = atoi(ll_args[1]);

	map_camp = map_mem(midaCamp);
	map_fi1 = map_mem(fi1);
	map_fi2 = map_mem(fi2);

	nfil = atoi(ll_args[8]);
	ncol = atoi(ll_args[9]);

	win_set(map_camp,nfil,ncol);

	while (!(*map_fi1) && !(*map_fi2)){
			nd = 0;
  for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
  {
    vk = (actual.d + k) % 4;		/* direccio veina */
    if (vk < 0) vk += 4;		/* corregeix negatius */
    seg.f = actual.f + df[vk]; /* calcular posicio en la nova dir.*/
    seg.c = actual.c + dc[vk];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
    if ((seg.a==' ') || (seg.a=='.') || (seg.a=='C'))
    { vd[nd] = vk;			/* memoritza com a direccio possible */
      nd++;
    }
  }
  if (nd == 0)				/* si no pot continuar, */
  	actual.d = (actual.d + 2) % 4;		/* canvia totalment de sentit */
  else
  { if (nd == 1)			/* si nomes pot en una direccio */
  	actual.d = vd[0];			/* li assigna aquesta */
    else				/* altrament */
    	actual.d = vd[rand() % nd];		/* segueix una dir. aleatoria */

			waitS(id_sem);
    seg.f = actual.f + df[actual.d];  /* calcular seguent posicio final */
    seg.c = actual.c + dc[actual.d];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
		if ((seg.a==' ') || (seg.a=='.') || (seg.a=='C'))
		{
    win_escricar(actual.f,actual.c,actual.a,NO_INV);	/* esborra posicio anterior */
    win_escricar(seg.f,seg.c,'0'+(intptr_t) index,NO_INV);		/* redibuixa fantasma */
		 actual.f = seg.f; actual.c = seg.c; actual.a = seg.a;	/* actualitza posicio */
	 }
		 signalS(id_sem);
    if (actual.a == 'C') *map_fi2 = 1;		/* ha capturat menjacocos */
  }
	win_retard(retard*2);
   }
  return(0);
}
