#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per getpid() */
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
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

int df[] = {-1,0,1,0};
int dc[] = {0,-1,0,1};

int prismatics(int *col_cocos, int *fil_cocos, int col_fantasma, int fil_fantasma, int dir_fantasma){
	int retornar=0;
 char casella = win_quincar((fil_fantasma + df[dir_fantasma]),(col_fantasma + dc[dir_fantasma]));
	while((casella != '+') && (casella != 'C')){
		fil_fantasma=fil_fantasma + df[dir_fantasma];
		col_fantasma=col_fantasma + dc[dir_fantasma];
		casella = win_quincar((fil_fantasma + df[dir_fantasma]),(col_fantasma + dc[dir_fantasma]));
	}

	if(casella=='C'){
		retornar=1;
		*col_cocos=col_fantasma;
		*fil_cocos=fil_fantasma;
	}else{
			retornar=0;
	}
	return retornar;
}

int main(int n_args, const char *ll_args[])
{

	int index_bustia, colm,film;
	char missatge[10], resposta[100];
	intptr_t *map_fi1 , *map_fi2, *map_camp, *map_xocs;
	int *map_bustia;
	int midaCamp, id_sem, fi1, fi2, xocs, nfil, ncol, retard, index, id_bustia_mem;
  int k, vk, nd, vd[3];
	objecte seg, actual;
	int columnaa, filaa, envioo=10;
	//int direccio, restaFiles, restaColumnes;

	int direccio =0;
	int n_threads = atoi(ll_args[15]);
	fi1 = atoi(ll_args[2]);
	fi2 = atoi(ll_args[3]);
	map_fi1 = map_mem(fi1);
	map_fi2 = map_mem(fi2);

	index=atoi(ll_args[11]);
	xocs=atoi(ll_args[14]);
	map_xocs = map_mem(xocs);
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
	int vist = 0, activat=0;

	while (!(*map_fi1) && !(*map_fi2)){
			nd = 0;
			vist = prismatics(&colm, &film, actual.c,actual.f,actual.d);
			columnaa=colm;
			filaa=film;
			if(vist==1){
				sprintf(missatge, "%i %i %i ",vist,colm, film);
				for(index_bustia=0; index_bustia<	n_threads;index_bustia++ ){
					sendM(map_bustia[index_bustia], missatge, 10);
				}
				activat=1;
			}else{
				if(vist==0 && activat==1){
					sprintf(missatge, "%i %i %i ",vist,colm, film);
					for(index_bustia=0; index_bustia<	n_threads;index_bustia++ ){
						sendM(map_bustia[index_bustia], missatge, 10);
					}
					activat=0;
				}
			}
			sprintf(missatge, "%i %i %i ",(index+10),colm, film);
			sendM(map_bustia[index], missatge, 10);
			int bucle=0;
			int rebut = receiveM(map_bustia[index], resposta);
			sscanf(resposta, "%i %i %i ", &envioo ,&columnaa ,&filaa);
			fprintf(stderr,"Soc el fantasma %i\nLleigeixo: %i %i %i\nBucle %i\nbytes = %i\n\n",index, envioo, columnaa, filaa,bucle, rebut );
			for(bucle=0; envioo != (index+10); bucle++){
					if(envioo==0){
						vist=0;
					}else{
						if(envioo==1){
							vist=1;
							if(fabs(actual.c-columnaa)>fabs(actual.f-filaa)){
								if((actual.c-columnaa)>=0){
									direccio=1;
								}else{
									if((actual.c-columnaa)<0){
										direccio=3;
									}
								}
							}else{
								if((actual.f-filaa)>0){
									direccio=0;
								}else{
									if((actual.f-filaa)<0){
										direccio=2;
									}
								}
							}
						}
					}
					receiveM(map_bustia[index], resposta);
					sscanf(resposta, "%i %i %i ", &envioo ,&columnaa ,&filaa);
					fprintf(stderr,"Soc el fantasma %i\nLleigeixo: %i %i %i\nBucle %i\n\n",index, envioo, columnaa, filaa,(bucle+1) );
				}


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
			if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C') || (seg.a== '+')) && (vist==1) && (nfil-2>seg.f) && (0<seg.f) && (ncol-2>seg.c) && (0<seg.c))
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
			if(vist==0){
	  		seg.f = actual.f + df[actual.d];  /* calcular seguent posicio final */
	  		seg.c = actual.c + dc[actual.d];
			}else{
				actual.d = direccio;
				seg.f = actual.f + df[direccio];  /* calcular seguent posicio final */
	  		seg.c = actual.c + dc[direccio];
				seg.a=win_quincar(seg.f,seg.c);
			}

    seg.a = win_quincar(seg.f,seg.c);	/* calcular caracter seguent posicio */
		if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C')) && (vist==0))
		{
			if(actual.a == '+'){
				win_escricar(actual.f,actual.c,actual.a,INVERS);
			}else{
			win_escricar(actual.f,actual.c,actual.a,NO_INV);	/* esborra posicio anterior */
		}
    win_escricar(seg.f,seg.c,'0'+(intptr_t) index,NO_INV);		/* redibuixa fantasma */
		 actual.f = seg.f; actual.c = seg.c; actual.a = seg.a;	/* actualitza posicio */
	 }else{
		 if (((seg.a==' ') || (seg.a=='.') || (seg.a=='C') || (seg.a=='+')) && (vist==1)&& (nfil-2>seg.f) && (0<seg.f) && (ncol-2>seg.c) && (0<seg.c))
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
	win_retard(retard*2);
}
  return(0);
}
