/*
 *                       Prova Finale                       *
 *           Algoritmi e Principi dell'Informatica          *
 *                 Anno Accademico 2017-2018                *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HASH 601		//numero primo non troppo vicino ad una potenza esatta di 2
#define DIM 64

unsigned int MAX; 		//da iniziallizare con standard in
unsigned int max;		//le mosse sono allo stesso numero per tutti gli OC, ne basta uno!
unsigned long lenght=5,*Len;
struct hash *H[HASH];
struct acc *A=NULL;
short out;
char error;

struct acc {
	int q;
	struct acc *next;
};

struct cell {		//cella del nastro di ingresso bidirezionale
	char car[DIM];
	unsigned int gen;
	double life;
	struct cell *next;
	struct cell *prec;
};

struct oc {			//singolo organo di controllo, poi da fare in lista
	unsigned long int q;		//tiene conto dello stato
	short int pos;
	struct cell *tape;
	struct oc *next;
};

struct hash {
	unsigned long int now, prox;
	short flag;
	char read, write, lrs;
	unsigned int key;
	struct hash *next;
	struct hash *om;
};

unsigned int genKEY (int q, char read) {
	unsigned int k, i;
	i= (int) read;
	k=q*100;
	k+=i;
	return k;
}

struct hash *search(struct hash* H[], unsigned int k) {	//passo tabella hash, elemento da cercare e valore funz. hash
	struct hash *temp;
	int f;
	f=(k%HASH);
	temp=H[f];
	while(1) {
		if(temp==NULL||temp->key==k) return temp;		//può ritornare anche NULL
		else temp=temp->next;
	}
}

void insert_hash (struct hash* H[], struct hash *t) {
	int f;
	struct hash *temp;
	temp=search(H, t->key);
	if(temp==NULL) {
		f=(t->key%HASH);
		t->next=H[f];	//inserisci in testa
		H[f]=t;
	}
	else {
		if(t->prox==temp->prox&&t->write==temp->write) return;	//evito di inserire duplicati (raro, forse impossibile)
		else {
			t->om=temp->om;
			temp->om=t;
		}
	}
	return;
}

void controll (struct hash* H[]) {
	struct hash *temp, *temp2;
	short int i, wout=0;
	Len=calloc(lenght, sizeof(unsigned long));
	for(i=0; i<HASH; i++) {
		temp=H[i];
		while(temp!=NULL) {
			if(temp->om==NULL) {
				if(temp->now==temp->prox) {
					if (temp->read=='_') {
						temp->lrs='S';
						temp->write='_';
					}
					if(temp->read==temp->write&&temp->lrs=='S') temp->lrs='U';
				}
			}
			if(temp->now!=temp->prox) Len[temp->now]++;
			temp2=temp->om;
			while(temp2!=NULL) {
				if(temp2->now!=temp2->prox) Len[temp2->now]++;
				else if(temp2->read==temp2->write&&temp2->lrs=='S') temp2->lrs='U';
				temp2=temp2->om;
			}
		 	temp=temp->next;
		}
	}
	if(lenght<1200) {
		while(wout==0) {
			wout=1;
			for(i=0; i<HASH; i++) {
				temp=H[i];
				while(temp!=NULL) {
					if(temp->now!=temp->prox) {
						if(Len[temp->prox]==0&&temp->flag==0) {
							temp->flag=1;
							wout=0;
							if(temp->om!=NULL) Len[temp->now]--;
						}
					}
					temp2=temp->om;
					while(temp2!=NULL) {
						if(temp2->now!=temp2->prox) {
							if(Len[temp2->prox]==0&&temp2->flag==0) {
								temp2->flag=1;
								wout=0;
								Len[temp2->now]--;
							}
						}
						temp2=temp2->om;
					}
					temp=temp->next;
				}
			}
		}
	}
	free(Len);
	return;
}

struct cell *create_cell (struct cell *C, int i) {	//creato ed inizializzato a blank, *C è quello a cui si attacca
	struct cell *temp;
	temp=malloc(sizeof(struct cell));
	temp->life=1;
	if(i==0) {									//sinistra
		temp->next=C;
		temp->prec=NULL;
		if (C!=NULL) {
			temp->gen=C->gen;
			C->prec=temp;
		}
	}
	else {										//destra
		temp->next=NULL;
		temp->prec=C;
		if (C!=NULL) {				//Per evitare errori in split, è corretto così se non capisci cazzi tuoi!
			temp->gen=C->gen;
			C->next=temp;
		}
	}
	return temp;
}

struct oc *create_oc(struct oc *L) {		//gli passo l'OC da cui si splitta, per poter copiarne i dati
	struct oc *temp;
	temp=malloc(sizeof(struct oc));
	temp->q=L->q;
	temp->pos=L->pos;
	temp->next=L->next;
	L->next=temp;
	return temp;
}

struct hash *create_hash () {	//ritorna la foglia creata
	struct hash *temp;
	int i;
	temp=malloc(sizeof(struct hash));
	temp->next=NULL;
	temp->om=NULL;
	temp->flag=0;
	return temp;
}

void iniz_tape(struct oc *L) {
	struct cell *temp;
	char c;
	int j;
	error=getchar();
	if(error==EOF) {
		out=2;
		L->tape=NULL;
		return;
	}
	temp=malloc(sizeof(struct cell));
	for(j=0; j<DIM&&error!=EOF&&error!='\n'; j++) {
		temp->car[j]=error;
		error=getchar();
	}
	temp->next=NULL;
	temp->prec=NULL;
	temp->life=1;
	temp->gen=0;
	L->tape=temp;
	while(error!=EOF&&error!='\n') {
		temp=create_cell(temp, 1);		//crea sempre a dx di se stessa
		for(j=0; j<DIM&&error!=EOF&&error!='\n'; j++) {
			temp->car[j]=error;
			error=getchar();
		}
	}
	for(j; j<DIM; j++) {
		temp->car[j]='_';
	}
	return;
}

struct acc *iniz_acc() {				//ritorna il primo della lista
	struct acc *temp1, *temp, *L;
	int i;
	char c[4];
	scanf("%d", &i);
	temp=malloc(sizeof(struct acc));
	temp->q=i;
	temp->next=NULL;
	L=temp;
	while(1) {
		scanf("%s", c);
		if(strcmp(c, "max")==0) return L;
		else {
			i=atoi(c);
			temp=malloc(sizeof(struct acc));
			temp->q=i;
			temp->next=L;
			L=temp;
		}
	}
}

void iniz_hash(struct hash* H[]) {
	struct hash *temp;
	char c[9];
	int i;

	for(int i=0; i<HASH; i++) {
		H[i]=NULL;
	}
	while(1) {
		scanf("%s", c);
	 	if(strcmp(c, "acc")!=0) {
			temp=create_hash();
	 		temp->now=strtoul(c, NULL, 10);
			while((temp->read=getchar())==' ');
			while((temp->write=getchar())==' ');
			while((temp->lrs=getchar())==' ');
			scanf(" %ld", &temp->prox);
			temp->key=genKEY(temp->now, temp->read);
			temp->next=NULL;
			if(temp->now>lenght) lenght= temp->now;
			if(temp->prox>lenght) lenght= temp->prox;
			insert_hash(H, temp);
		}
		else {
			lenght++;
			controll(H);
			return;
		}
	}
}

short int split(struct oc *L, struct hash *H) {	//gli passo l'OC da cui si splitta, la cella hash (H->om !!!) per sapere quante volte si splitta
	struct cell *newCELL, *oldCELL;						//gli deve passare H->om, sennò me li divide una volta in più (oppure cambiare if(H->om==NULL))
	struct oc *newOC;
	struct hash *temp=NULL;
	int i=0, j;

	temp=H;
	while(H->om!=NULL) {
		if(temp->flag!=-1&&H->om->flag!=-1) {
			i++;
			newCELL=create_cell(L->tape->prec, 1);		//nastro per il nuovo oc
			oldCELL=L->tape;						//nastro per il vecchio oc che va duplicato
			newOC=create_oc(L);							//nuovo OC		
			for(j=0; j<DIM; j++) {
				newCELL->car[j]=L->tape->car[j];
			}
			newCELL->next=L->tape->next;
			//newCELL->prec già fatto alla creazione!
			newCELL->gen=L->tape->gen+1;
			oldCELL->gen=L->tape->gen+1;
			newOC->tape=newCELL;
			if(L->tape->prec!=NULL) L->tape->prec->next=NULL;
			if(L->tape->next!=NULL) L->tape->next->prec=NULL;
		}
		if(temp->flag==-1) temp=temp->om;
		H=H->om;
	}
	return i;
}

void dup(struct cell *x, int i) {
	struct cell *temp, *del;
	int gen, j;
	if(i==0) {
		del=x->prec;
		gen= x->gen - del->gen;
		del->life -= pow(2, -gen);
		if(del->life!=0) {
			temp=create_cell(x, 0);
			for(j=0; j<DIM; j++) {
				temp->car[j]=del->car[j];
			}
			temp->prec=del->prec;
		}
		else {		//invece di eliminarlo lo riciclo
			del->next=x;
			del->gen=x->gen;
			del->life=1;
			if(del->prec!=NULL)	del->prec->next=NULL;
		}
	}
	else { 	//if(i==1)
		del=x->next;
		gen= x->gen - del->gen;
		del->life -= pow(2, -gen);
		if(del->life!=0) {
			temp=create_cell(x, 1);
			for(j=0; j<DIM; j++) {
				temp->car[j]=del->car[j];
			}
			temp->next=del->next;
		}
		else {
			del->prec=x;
			del->gen=x->gen;
			del->life=1;
			if(del->next!=NULL) del->next->prec=NULL;
		}
	}
	return;
}

void pre_delete(struct cell* L1, struct cell* L2, double damage1, double damage2, short int gen_original) {
	struct cell *temp1, *temp2;
	int gen, loop1, loop2;
	while(L1!=NULL||L2!=NULL) {
		loop1=0; loop2=0;
		temp1=L1;
		if(L1!=NULL) L1=L1->next;
		temp2=L2;
		if(L2!=NULL) L2=L2->prec;
		if(L1!=NULL) {
			if(L1->gen!=temp1->gen&&loop1==0) {	//Gradini
				temp1->life= temp1->life - damage1;
				if (temp1->life==0) {
					free(temp1);
				}
				gen= gen_original - L1->gen;
				damage1= pow(2, -gen);
				loop1=1;
			}
			else if(loop1==0) {
				temp1->life= temp1->life - damage1;
				if (temp1->life==0) {
					free(temp1);
				}
				loop1=1;
			}
		}
		else if(temp1!=NULL&&loop1==0) {					//ultimo del nastro
			temp1->life= temp1->life - damage1;
			if (temp1->life==0) {
				free(temp1);
			}
		}
		if(L2!=NULL) {
			if(L2->gen!=temp2->gen&&loop2==0) {	//Gradini
				temp2->life= temp2->life - damage2;
				if (temp2->life==0) {
					free(temp2);
				}
				gen= gen_original - L2->gen;
				damage2= pow(2, -gen);
				loop2=1;
			}
			else if(loop2==0) {
				temp2->life= temp2->life - damage2;
				if (temp2->life==0) {
					free(temp2);
				}
				loop2=1;
			}
		}
		else if(temp2!=NULL&&loop2==0) {					//ultimo del nastro
			temp2->life= temp2->life - damage2;
			if (temp2->life==0) {
				free(temp2);
			}
		}
	}
}

void delete(struct cell *C) {
	struct cell *temp2, *L1, *L2;
	int gen2, gen_original;
	double damage1=1, damage2=1;
	L1=C;
	L2=C->prec;
	gen_original=C->gen;			//Al secondo gradino devo ricalcolare il danno tra la gen_originale e quella del nuovo livello
	if(L2!=NULL&&L2->gen!=L1->gen) {	//se scendo SUBITO dal gradino
		gen2= gen_original - L2->gen;
		damage2= pow(2, -gen2);
		temp2=L2;
		L2=L2->prec;
		temp2->life= temp2->life - damage2;
		if(L2!=NULL&&L2->gen!=temp2->gen) {
			gen2= gen_original - L2->gen;
			damage2= pow(2, -gen2);
		}
		if(temp2->life==0) {
			free(temp2);
		}
	}
	pre_delete(L1, L2, damage1, damage2, gen_original);
	return;
}

void smash(struct cell *C) {
	struct cell *temp, *L1, *L2;
	L1=C;
	L2=C->prec;
	while(L1!=NULL) {
		temp=L1;
		L1=L1->next;
		if(temp->life>0) free(temp);
	}
	while(L2!=NULL) {
		temp=L2;
		L2=L2->prec;
		if(temp->life>0) free(temp);
	}
	return;
}

void boom(struct oc *L) {
	struct oc *temp;
	struct hash *temp1, *temp2;
	while(L!=NULL) {
		if(out!=2) {
			if(L->next==NULL) smash(L->tape);
			else delete(L->tape);	//!=NULL perchè l'OC dell'ultimo giro è NULL
		}
		temp=L;
	 	L=L->next;
	 	free(temp);
	}
	if(error!=EOF) {
		for(int i=0; i<HASH; i++) {
			temp1=H[i];
			while(temp1!=NULL) {
				if(temp1->flag==-1) temp1->flag=1;
				temp2=temp1->om;
				while(temp2!=NULL) {
					if(temp2->flag==-1) temp2->flag=1;
					temp2=temp2->om;
				}
				temp1=temp1->next;
			}
		}
	}
	return;
}

void main() {
	struct acc *tempA;
	struct cell *current_cell;
	struct hash *delta, *cache_delta[2]={NULL, NULL};
	struct oc *O=NULL, *O1, *tempO, *pre;
	char c[7], cache_read[2];
	short int n_split, cache_q[2]={-1, -1}, cache_flag=0, i, j, pos, U, U_flag;
	unsigned int num_key;
	
	scanf("%s\n", c);
	iniz_hash(H);
	A=iniz_acc();
	scanf("%d", &MAX);
	scanf("%s\n", c);		//run
	while(error!=EOF) {
		O=malloc(sizeof(struct oc));
		O->q=0;
		O->pos=0;
		O->next=NULL;
		iniz_tape(O);
		max=0;
		if(out!=2) out=-1;
		U=0;
		while(max<MAX&&out==-1) {
			max++;
			O1=O;
			pre=NULL;
			while(O1!=NULL&&out==-1) {
				current_cell=O1->tape;
				pos=O1->pos;
				if(O1->q==cache_q[0]&&current_cell->car[pos]==cache_read[0]) delta=cache_delta[0];		//LEGGO
				else if(O1->q==cache_q[1]&&current_cell->car[pos]==cache_read[1]) delta=cache_delta[1];
				else {
					num_key=genKEY(O1->q, current_cell->car[pos]);
					delta=search(H, num_key);
					if(delta!=NULL) {	//aggiorna cache
						if(cache_flag==0) {
							cache_flag=1;
							cache_delta[0]=delta;
							cache_read[0]=current_cell->car[pos];
							cache_q[0]=O1->q;
						}
						else {
							cache_flag=0;
							cache_delta[1]=delta;
							cache_read[1]=current_cell->car[pos];
							cache_q[1]=O1->q;
						}
					}
				}
				if(delta!=NULL) {
					if(delta->om!=NULL) n_split= split(O1, delta); //ritorna il numero di volte che si è diviso
					else n_split=0;
					for(i=0; i<=n_split; i++) {
						while(delta!=NULL&&delta->flag==-1) { delta=delta->om; }
						if(delta!=NULL) {
							U_flag=0;
							if(delta->flag==1) delta->flag=-1;
							current_cell=O1->tape;
							pos=O1->pos;
							current_cell->car[pos]=delta->write;			//SCRIVO
							O1->q=delta->prox;
							if(delta->lrs=='R') {							//SPOSTO
								if(pos<DIM-1) O1->pos++;
								else {
									if(current_cell->next==NULL) {
										create_cell(current_cell, 1);
										for(j=0; j<DIM; j++) {
											current_cell->next->car[j]='_';
										}
									}
									else if(current_cell->next->gen!=current_cell->gen) dup(current_cell, 1);
									O1->tape=O1->tape->next;
									O1->pos=0;
								}
							}
							else if(delta->lrs=='L') {
								if(pos>0) O1->pos--;
								else {
									if(current_cell->prec==NULL) {
										create_cell(current_cell, 0);
										for(j=0; j<DIM; j++) {
											current_cell->prec->car[j]='_';
										}
									}
									else if(current_cell->prec->gen!=current_cell->gen) dup(current_cell, 0);
									O1->tape=O1->tape->prec;
									O1->pos=DIM-1;
								}
							}
							else if(delta->lrs=='U') {
								U=1;
								U_flag=1;
								delete(O1->tape);
								tempO=O1;
								if(pre==NULL) O=tempO->next;
								else pre->next=tempO->next;
								O1=O1->next;
								free(tempO);
								if(O==NULL) max=MAX;
								delta=delta->om;
								}
							if(U_flag==0) {
								delta=delta->om;
								pre=O1;
								O1=O1->next;
							}
						}
					}
				}
				else {
					tempA=A;
					while(tempA!=NULL&&out==-1) {
						if(tempA->q==O1->q) out=1;
						else tempA=tempA->next;
					}
					if(out==-1) {	//DEVO ELIMINARE LA LISTA DELL'OC INTERESSATO. SE ELIMINO TUTTI GLI OC ALLORA OUT=0
						delete(O1->tape);
						tempO=O1;
						if(pre==NULL) O=tempO->next;
						else pre->next=tempO->next;
						O1=O1->next;
						free(tempO);
						if(O==NULL) out=0;
					}
				}
			}
		}
	if(out==0&&U==0) printf("0\n");
	else if(out==1) printf("1\n");
	else if(max==MAX||U==1) printf("U\n");
	boom(O);
	}
return;
}