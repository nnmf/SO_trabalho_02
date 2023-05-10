#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUM_FILAS 4


typedef struct pcb{
    int id;
    int prioridade;
    int tempo;
} PCB;


typedef struct stno{
    PCB *processo;
    struct stno *prox;
} No;


typedef struct fila{
    No *inicio;
    No *fim;
    int quantum;
    int id;
} Fila;

void inicia_filas(Fila *q, PCB *p);
void imprime_filas(Fila **filas);
void adiciona_processo(Fila *q, PCB *p);
void remove_processo(Fila *q);
PCB *proximo_processo(Fila *filas);



int main() {
    Fila *filas[NUM_FILAS];
    int t[NUM_FILAS], i, auxiliar;

    //começo alocar dinamicamente as filas passando o quantum e o id, o quantum minimo sera de 2
    for(i = 0; i < NUM_FILAS; i++){
        filas[i] = malloc(sizeof(Fila));
        filas[i]->inicio = NULL;
        filas[i]->fim = NULL;
        filas[i]->quantum = i + 2;
        filas[i]->id = i + 1;
    }
    //para gerar numeros aleatorios, os numeros gerados serão o tempo, onde eu irei implementar no próximo for
    srand(time(NULL));

    //defino o id, a prioridade, e o tempo eu gero pseudo-aleatoriamente em um vetor, os ordeno de ordem crescente e os
    // passo para meu PCB que é colocado em uma fila e os printo
    for(i=0; i<NUM_FILAS; i++){

        for (int a = 0; a < 4; a++) {
            auxiliar = 1 + rand() % 20; // Gera um número aleatório entre 1 e 20

            // Insere o número gerado em ordem crescente no vetor
            for (int b = a; b >= 0; b--) {
                if (b == 0 || t[b-1] <= auxiliar) {
                    t[b] = auxiliar;
                    break;
                } else {
                    t[b] = t[b-1];
                }
            }
        }
        PCB p1 = {1, 1, t[0]};
        PCB p2 = {2, 2, t[1]};
        PCB p3 = {3, 3, t[2]};
        PCB p4 = {4, 4, t[3]};
        inicia_filas(filas[i], &p1);
        inicia_filas(filas[i], &p2);
        inicia_filas(filas[i], &p3);
        inicia_filas(filas[i], &p4);
    }
    imprime_filas(filas);

    //faço o escalonamento
    printf("\n ---------------------------\n Inicio do Escalonamento\n ---------------------------\n");
    int tempo_total = 0;
    for(int j=0; j < NUM_FILAS; j++){
        printf("|| Processo-ID || Fila || Tempo Restante || Quantum ||\n");
        while(1) {
            PCB *p = proximo_processo(filas[j]);
            if(p == NULL){
                break;
            }
            if(p->tempo <= filas[j]->quantum){
                tempo_total += p->tempo;
                printf("||   %d    ||   %d    ||   Finalizado  ||   %d    ||\n", p->id, filas[j]->id, filas[j]->quantum);
                remove_processo(filas[j]);
            }
            else{
                tempo_total += filas[j]->quantum;
                p->tempo -= filas[j]->quantum;
                printf("||   %d    ||   %d    ||      %d     ||   %d    ||\n", p->id, filas[j]->id, p->tempo, filas[j]->quantum);
                adiciona_processo(filas[j], p);
                remove_processo(filas[j]);
            }
        }
        printf("\n ---------------------------\nTempo total de executando: %d segundos\n ---------------------------\n", tempo_total);
        tempo_total=0;
    }
    printf("\n ---------------------------\n Fim do Escalonamento\n ---------------------------\n");

    //checo uma última vez se tem processos para remover e se tiver removo, de qualquer forma eu dou free nas filas
    //pois usei maloc lá em cima
    for(int i = 0; i < NUM_FILAS; i++){
        while(filas[i]->inicio != NULL){
            remove_processo(filas[i]);
        }
        free(filas[i]);
        filas[i] = NULL;
    }


    return 0;
}



//adiciono o processo de acordo com a prioridade dada
void inicia_filas(Fila *q, PCB *p) {
    No *auxiliar = malloc(sizeof(No));
    auxiliar->processo = malloc(sizeof(PCB));
    *auxiliar->processo = *p;
    if (q->inicio == NULL) {
        // se a fila estiver vazia, insira o novo processo no início
        auxiliar->prox = auxiliar;
        q->inicio = auxiliar;
        q->fim = auxiliar;
    } else if (p->prioridade < q->inicio->processo->prioridade) {
        // se a prioridade do novo processo for menor que a do processo na cabeça da fila,
        // insira o novo processo no início
        auxiliar->prox = q->inicio;
        q->fim->prox = auxiliar;
        q->inicio = auxiliar;
    } else {
        // encontra o lugar certo para inserir o novo processo na fila
        No *anterior = q->inicio;
        No *atual = q->inicio->prox;
        while (atual != q->inicio && p->prioridade >= atual->processo->prioridade) {
            anterior = atual;
            atual = atual->prox;
        }
        anterior->prox = auxiliar;
        auxiliar->prox = atual;
        if (atual == q->inicio) {
            q->fim = auxiliar;
        }
    }
}

void imprime_filas(Fila **filas) {
    for(int i = 0; i < NUM_FILAS; i++) {
        printf("\n ---------------------------\n Fila %d\n ---------------------------\n", filas[i]->id);
        printf("|| Processo-ID || Fila || Prioridade || Tempo Total || Quantum ||\n");

        if (filas[i]->inicio != NULL) {
            No *no_atual = filas[i]->inicio;

            do {
                printf("||     %d       ||  %d  ||     %d      ||     %d      ||   %d    ||\n", no_atual->processo->id, filas[i]->id, no_atual->processo->prioridade, no_atual->processo->tempo, filas[i]->quantum);
                no_atual = no_atual->prox;
            } while(no_atual != filas[i]->inicio);
        }
    }
}


//manipula os processos enquanto escalona
void adiciona_processo(Fila *q, PCB *p){
    No *auxiliar = malloc(sizeof(No));
    auxiliar->processo = malloc(sizeof(PCB));
    *auxiliar->processo = *p;
    if(q->fim == NULL){
        auxiliar->prox = auxiliar;
        q->inicio = auxiliar;
    }
    else{
        auxiliar->prox = q->inicio;
        q->fim->prox = auxiliar;
    }
    q->fim = auxiliar;
}


void remove_processo(Fila *q){
    if(q->inicio == q->fim){
        free(q->inicio);
        q->inicio = NULL;
        q->fim = NULL;
    }
    else{
        No *temp = q->inicio;
        q->inicio = q->inicio->prox;
        q->fim->prox = q->inicio;
        free(temp);
    }
}

PCB *proximo_processo(Fila *filas){
    if(filas->inicio != NULL){
        return filas->inicio->processo;
    }
    return NULL;
}