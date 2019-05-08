#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per getpid() */
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <curses.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
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

	char missatge[80], resposta[80], vistcocos[80];
	sprintf(vistcocos, "el cocos ha estat vist");
	intptr_t *map_fi1 , *map_fi2, *map_camp, *map_xocs;
	int *map_bustia;
	int midaCamp, id_sem, fi1, fi2, xocs, nfil, ncol, retard, index, id_bustia_mem;
  int k, vk, nd, vd[3];
	objecte seg, actual;
	int df[] = {-1,0,1,0};
	int dc[] = {0,-1,0,1};

	int n_threads = atoi(ll_args[15]);
	fi1 = atoi(ll_args[2]);
	fi2 = atoi(ll_args[3]);
	map_fi1 = map_mem(fi1);
	map_fi2 = map_mem(fi2);

	index=atoi(ll_args[11]);
	xocs=atoi(ll_args[14]);
	map_xocs = map_mem(xocs);
	sprintf(missatge, "%i",index);
	int auxiliar = 2 * (1+index);
	while(((*map_xocs) < auxiliar ) &&  !(*map_fi1) && !(*map_fi2)){

	}
	actual.a=ll_args[4][0];
	actual.c=atoi(ll_args[7]);
	actual.f=atoi(ll_args[6]);
	actual.d=atoi(ll_args[5]);

	retard=atoi(ll_args[10]);
	id_sem = atoi(ll_args[12]);
	midaCamp = atoi(ll_args[1]);
	id_bustia_mem = atoi(ll_args[13]);
	map_bustia = map_mem(id_bustia_mem);

	map_camp = map_mem(midaCamp);
	map_bustia = map_mem(id_bustia_mem);
	nfil = atoi(ll_args[8]);
	ncol = atoi(ll_args[9]);

	win_set(map_camp,nfil,ncol);
	srand(getpid());
	int vist = 0;
	int jo = 0;

	while (!(*map_fi1) && !(*map_fi2)){
			nd = 0;
  for (k=-1; k<=1; k++)		/* provar direccio actual i dir. veines */
  {
    vk = (actual.d + k) % 4;		/* direccio veina */
    if (vk < 0) vk += 4;		/* corregeix negatius */
    seg.f = actual.f + df[vk]; /* calcular posicio en la nova dir.*/
    seg.c = actual.c + dc[vk];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */

    if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C')) && (vist==0))
    { vd[nd] = vk;			/* memoritza com a direccio possible */
      nd++;
    }else{
			if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C') || (seg.a== '+')) && (vist==1) && (nfil-1>seg.f) && (0<seg.f) && (ncol-1>seg.c) && (0<seg.c))
	    { vd[nd] = vk;			/* memoritza com a direccio possible */
	      nd++;
	    }
		}
  }
  if (nd == 0){				/* si no pot continuar, */
  	actual.d = (actual.d + 2) % 4;		/* canvia totalment de sentit */
	}
  else
  { if (nd == 1){
				/* si nomes pot en una direccio */
				if((win_quincar((actual.f + df[actual.d]),(actual.c + dc[actual.d])))=='+'){
					if ((n_threads*2)>(*map_xocs)){
						*map_xocs = (*map_xocs)+1;
					}
				}
  	actual.d = vd[0];			/* li assigna aquesta */
	}else				/* altrament */
    	actual.d = vd[rand() % nd];		/* segueix una dir. aleatoria */

			waitS(id_sem);
    seg.f = actual.f + df[actual.d];  /* calcular seguent posicio final */
    seg.c = actual.c + dc[actual.d];
    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
		if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C')) && (vist==0))
		{
    win_escricar(actual.f,actual.c,actual.a,NO_INV);	/* esborra posicio anterior */
    win_escricar(seg.f,seg.c,'0'+(intptr_t) index,NO_INV);		/* redibuixa fantasma */
		 actual.f = seg.f; actual.c = seg.c; actual.a = seg.a;	/* actualitza posicio */
	 }else{
		 if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C') || (seg.a=='+')) && (vist==1)&& (nfil-1>seg.f) && (0<seg.f) && (ncol-1>seg.c) && (0<seg.c))
		 {
			if(actual.a == '+'){
				win_escricar(actual.f,actual.c,actual.a,INVERS);
			}else{
			win_escricar(actual.f,actual.c,actual.a,NO_INV);	/* esborra posicio anterior */
		}

		win_escricar(seg.f,seg.c,'0'+(intptr_t) index,INVERS);		/* redibuixa fantasma */
			actual.f = seg.f; actual.c = seg.c; actual.a = seg.a;	/* actualitza posicio */
		}
	 }
		 signalS(id_sem);
    if (actual.a == 'C') *map_fi2 = 1;		/* ha capturat menjacocos */
  }
	waitS(id_sem);
	int k;
	for(k=0; k<	n_threads;k++ ){
		sendM(map_bustia[k], vistcocos, 22);
	}

	sendM(map_bustia[index], missatge, 2);
	signalS(id_sem);
	while(jo == 0){
		receiveM(map_bustia[index], resposta);
		if(strcmp(missatge, resposta) == 0){
			jo=1;
		}else{
			if(strcmp(vistcocos, resposta) == 0){
				vist=1;
			}
		}
	}
	jo=0;
	win_retard(retard*2);
   }
  return(0);
}
