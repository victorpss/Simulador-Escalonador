#include <stdio.h>
#include <stdlib.h>

typedef struct process{
    int pid; /* ID do processo */
    int arrivalTime; /* Tempo de chegada */
    int executeTime; /* Tempo de execução */
    int remainingTime; /* Tempo restante */
    int IOremainingTime; /* Tempo restante da requisição IO */
    int IOqnt; /* Quantidade de I/O's que o processo faz */
    int* IOtime; /* Tempo(s) de E/S */
    int* IOtype; /* Tipo(s) de E/S */
    
    struct process *next;

} process;

typedef struct queue{
   process *front, *back;

} queue;

queue* initQueue(){
    queue* fila = (queue*) malloc(sizeof(queue)); /* Aloca espaço na memória para a fila */
    fila->front = fila->back = NULL; /* Fila está vazia */

    return fila;
}

process* initProcess(int pid, int arrivalTime, int executeTime, int IOtime[], int IOtype[], int IOqnt){ /* função para iniciar um processo */
    int i;
    process* p = (process*)malloc(sizeof(process));

    p->pid = pid;
    
    p->arrivalTime  = arrivalTime;
    p->executeTime = executeTime;
    p->remainingTime = executeTime;

    p->IOqnt = IOqnt;
    p->IOtime = NULL;
    p->IOtype = NULL;

    if(IOqnt>0){
        p->IOtime = (int*) malloc (IOqnt * sizeof(int));
        p->IOtype = (int*) malloc (IOqnt * sizeof(int));

        for(i=0; i<IOqnt; i++){
            p->IOtime[i] = IOtime[i];
            p->IOtype[i] = IOtype[i];
        }
    }

    p->next = NULL;

    return p;
}

int isEmpty(queue* fila){ /* checa se a fila está vazia */
    return (fila->front==NULL);
}

int calculateIO(int num){
    switch(num){
        case 0:
            return 4;   /* tempo do Disco */
        case 1:
            return 8;   /* tempo da Fita */
        case 2:
            return 12;  /* tempo da Impressora */
        default:
            return 0;            
    }
}

void push(queue* fila, process* p){ /* coloca um processo no último lugar da fila */
    if(isEmpty(fila)){
        fila->front = fila->back = p;
        return;
    }

    fila->back->next = p;
    fila->back = p;
}

void pop(queue* fila){ /* retira o primeiro processo da fila */
    fila->front = fila->front->next;

    if(fila->front==NULL){
        fila->back = NULL;
    }
}

void printQueue(queue* fila){ /* realiza a impressão de uma fila */
    process* p = fila->front;

    while(p!=NULL){
        printf("%d ", p->pid);
        p = p->next;
    }
    printf("\n");
}

int displayQueues(int currentTime, int quantum, process* p, queue* highPriority, queue* lowPriority, queue* diskIOqueue, queue* magneticTapeIOqueue, queue* printerIOqueue){ /* mostra as filas e informações sobre os processos que estão em primeiro na fila */
    
    int queuesEmpty = 0;

    printf("Tempo: %d / Quantum: %d\n\n", currentTime, quantum);

    if(p==NULL){ /* p é o processo atual na CPU. Se é nulo, a CPU está sem processos */
        printf("Fila da CPU está vazia!\n\n");
        queuesEmpty++; /* As filas de alta e baixa prioridade estão vazias */
    }
    else{
        printf("ID do processo atual da CPU: %d\n", p->pid);
        printf("Tempo de serviço faltante: %d\n", p->remainingTime);
        printf("Fila de Alta Prioridade: ");
        printQueue(highPriority);
        printf("Fila de Baixa Prioridade: ");
        printQueue(lowPriority);
        printf("\n");
    }

    if(!isEmpty(diskIOqueue)){
        process* atual = diskIOqueue->front;
        printf("ID do processo atual do Disco: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n", atual->IOremainingTime);
        printf("Fila do Disco: ");
        printQueue(diskIOqueue);
        printf("\n");
    }
    else{
        printf("Fila do Disco está vazia!\n\n");
        queuesEmpty++;
    }

    if(!isEmpty(magneticTapeIOqueue)){
        process* atual = magneticTapeIOqueue->front;
        printf("ID do processo atual da Fita magnética: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n", atual->IOremainingTime);
        printf("Fila da Fita Magnética: ");
        printQueue(magneticTapeIOqueue);
        printf("\n");

    }else{
        printf("Fila da Fita magnética está vazia!\n\n");
        queuesEmpty++;
    }

    if(!isEmpty(printerIOqueue)){
        process* atual = printerIOqueue->front;
        printf("ID do processo atual da Impressora: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n", atual->IOremainingTime);
        printf("Fila da Impressora: ");
        printQueue(printerIOqueue);
        printf("\n");

    }else{
        printf("Fila da Impressora está vazia!\n\n");
        queuesEmpty++;
    }
    
    return queuesEmpty;
}

void displayPlot(int historyTime[], int historyType[], int processIdx){ /* função para realizar o display do gráfico final dos processos */
    int i, j;

    printf("Gráfico dos Processos:\n\n");
    
    for(i=0; i<processIdx; i++){
        for(j=0; j<historyTime[i]+1; j++){
            printf(" ");
        }
        if(historyType[i]!=-1) printf("%d", historyType[i]);
        else printf(" ");
        for(j=0; j<historyTime[i]+1; j++){
            printf(" ");
        }
        printf("   ");
    }
    
    printf("\n");

    for(i=0; i<processIdx; i++){
        printf("|");
        for(j=0; j<historyTime[i]; j++){
            printf(" ");
        }
        printf("%d", historyTime[i]);
        for(j=0; j<historyTime[i]; j++){
            printf(" ");
        }
        printf("|");
        printf("   ");
    }

    printf("\n\n");
}

void displayTurnarounds(int n, int turnaround[]){ /* função para realizar o display dos turnaround dos processos */
    int i, soma=0;
    for(i=0; i<n; i++){
        printf("Turnaround do Processo %d: %d\n", i, turnaround[i]);
        soma+=turnaround[i];
    }
    printf("\n");
    printf("Turnaround médio: %.2f\n", 1.0*soma/n);
}

void displayProcess(int n, queue* fila){ /* função para realizar o display dos processos no início do escalonamento */
    int i, j;
    process* p = fila->front;

    printf("Os seguintes processos serão escalonados: \n\n");

    for(i=0; i<n; i++){
        printf("Processo %d\n", i);
        printf("Tempo de ativação: %d\n", p->arrivalTime);
        printf("Tempo de serviço: %d\n", p->executeTime);
        if(p->IOqnt>0){
            printf("Tipos de I/0: ");
            for(j=0; j<p->IOqnt-1; j++){
                if(p->IOtype[j]==0){
                    printf("Disco, ");
                }else if(p->IOtype[j]==1){
                    printf("Fita magnética, ");
                }else{
                    printf("Impressora, ");
                }
            }
            if(p->IOtype[p->IOqnt-1]==0){
                printf("Disco\n");
            }else if(p->IOtype[j]==1){
                printf("Fita magnética\n");
            }else{
                printf("Impressora\n");
            }

            printf("Tempos de I/0: ");
            for(j=0; j<p->IOqnt-1; j++){
                printf("%d, ", p->IOtime[j]);
            }

            printf("%d\n", p->IOtime[p->IOqnt-1]);

            printf("\n");
        }else{
            printf("Não possui requisições de I/O!\n\n");
        }
        printf("\n");
        p = p->next;
    }
    printf("\nEnter para continuar >>");
    fflush(stdin);
    getc(stdin);
}

void scheduling(){
    int N = 5; /* quantidade de processos */

    /* Novos processos */
    queue* newProcess = initQueue();

    /* Processos prontos */
    queue* highPriority = initQueue();
    queue* lowPriority = initQueue();
    
    /* Processos bloqueados */
    queue* diskIOqueue = initQueue();
    queue* magneticTapeIOqueue = initQueue();
    queue* printerIOqueue = initQueue();

    int i;

    int quantum = 3; /* fatia de tempo estabelecida para cada processo */
    
    int currentTime; /* tempo atual do escalonador */
    
    int completedProcess = 0; /* quantidade de processos finalizados */

    int quantumAtual=0; /* por quantas u.t. seguidas o processo tá sendo executado na CPU */
    
    int high; /* 1 - estou executando alguém que veio da fila de alta; 0 - estou executando alguém que veio da baixa */

    int wasNull=0, lastNull=0; /* usado para quando não há nenhum processo na CPU */

    int changed, changedID; /* utilizado quando há uma troca de processos */

    int processIdx=0, historyTime[1000], historyType[1000]; /* para visualização do gráfico no fim do escalonamento */

    int turnaround[5]; /* guarda o turnaround dos processos */

    int currentIO[5]; /* indica o índice da próxima I/O que o processo deve realizar */
    
    process* currentProcess; /* atual processo da CPU */

    /* IOtime contém as unidades de tempo do processo em execução onde há requisição I/O */
    /* IOtype contém os tipos dessas requisições (Disco - 0; Fita magnética - 1; Impressora - 2) */

    int IOtime0[] = {2, 4};
    int IOtype0[] = {0, 1};

    int* IOtime1 = NULL;
    int* IOtype1 = NULL;

    int IOtime2[] = {5};
    int IOtype2[] = {2};

    int IOtime3[] = {6, 8};
    int IOtype3[] = {2, 0};

    int IOtime4[] = {3, 5};
    int IOtype4[] = {1, 1};
    
    /* iniciando processos */
    process* p0 = initProcess(0, 0, 7, IOtime0, IOtype0, 2);
    process* p1 = initProcess(1, 2, 6, IOtime1, IOtype1, 0);
    process* p2 = initProcess(2, 4, 9, IOtime2, IOtype2, 1);
    process* p3 = initProcess(3, 5, 11, IOtime3, IOtype3, 2);
    process* p4 = initProcess(4, 5, 7, IOtime4, IOtype4, 2);

    /* processos precisam ser colocados em ordem crescente de "tempo de ativação" na fila de novos processos */
    /* inserindo na fila de novos processos */

    push(newProcess, p0);
    push(newProcess, p1);
    push(newProcess, p2);
    push(newProcess, p3);
    push(newProcess, p4);

    /* apresentando as informações dos processos */
    displayProcess(N, newProcess);

    /* definindo para cada processo o índice inicial de I/O */
    for(i=0; i<N; i++){
        currentIO[i]=0;
    }

    /* iniciando o escalonamento */
    for(currentTime = 0; ; currentTime++){
        #ifdef _WIN32
            system("cls");
        #else 
            system("clear");
        #endif
        
        changed = 0;

        /* Verificação da fila de novos */
        while(!isEmpty(newProcess) && currentTime == newProcess->front->arrivalTime){  /* se na unidade de tempo atual chegou algum processo (no critério de desempate, novos processos entram antes) */
            process* temp = newProcess->front;
            pop(newProcess);  /* tira esse processo da fila de novos */
            temp->next = NULL; /* tira a referência ao próximo processo a ficar pronto */
            push(highPriority, temp);  /* bota na fila de alta prioridade */
        }

        if(!quantumAtual){ /* se anteriormente não tinha nenhum processo na CPU, vamos buscar o processo novo para ser executado */
            if(!isEmpty(highPriority)){ /* pego o novo processo da fila de alta prioridade, se tem alguém lá */
                high = 1;
                currentProcess = highPriority->front;
            }else if (!isEmpty(lowPriority)){  /* caso contrário, pego da baixa */
                high = 0;
                currentProcess = lowPriority->front;
            }
            else{ /* se as filas de alta e baixa estão vazias */
                currentProcess = NULL;
            }
        }

        displayQueues(currentTime, quantumAtual, currentProcess, highPriority, lowPriority, diskIOqueue, magneticTapeIOqueue, printerIOqueue);

        if(currentProcess!=NULL){
            int pid = currentProcess->pid;

            int nextIOtime, nextIOtype; /* tempo e tipo do próximo I/O do processo */

            if(wasNull){ /* se não tinha processo anterior na CPU */
                historyTime[processIdx] = currentTime-lastNull;
                historyType[processIdx] = -1;
                processIdx++;
            }

            wasNull = 0;
            
            if(currentIO[pid]<currentProcess->IOqnt){ /* se ainda possuir I/O para ser feito */
                nextIOtype = currentProcess->IOtype[currentIO[pid]]; /* tipo do próximo IO */
                nextIOtime = currentProcess->IOtime[currentIO[pid]]; /* próximo tempo do IO do processo atual */
            }else{
                nextIOtime = -1;
            }

            if(currentProcess->executeTime - currentProcess->remainingTime == nextIOtime){ /*se chegou o tempo do próximo IO*/
                historyType[processIdx] = pid;
                historyTime[processIdx] = quantumAtual;
                processIdx++;

                high ? pop(highPriority) : pop(lowPriority);
                currentProcess->next = NULL;
                
                if(nextIOtype == 0){
                    currentProcess->IOremainingTime = calculateIO(nextIOtype);
                    push(diskIOqueue, currentProcess);
                }
                else if (nextIOtype == 1){
                    currentProcess->IOremainingTime = calculateIO(nextIOtype);
                    push(magneticTapeIOqueue, currentProcess);
                }
                else{
                    currentProcess->IOremainingTime = calculateIO(nextIOtype);
                    push(printerIOqueue, currentProcess);
                }

                changed = 1;
                changedID = pid;

                quantumAtual=0;
            }
            else if(!currentProcess->remainingTime){ /* se um processo terminou a execução */
                historyType[processIdx] = pid;
                historyTime[processIdx] = quantumAtual;
                processIdx++;

                high ? pop(highPriority) : pop(lowPriority);

                turnaround[currentProcess->pid] = currentTime - currentProcess->arrivalTime;

                completedProcess++;
                free(currentProcess);

                changed = 2;
                changedID = pid;

                quantumAtual=0;
            }
            else if(quantumAtual == quantum){ /* se esgotou a fatia de tempo, o processo sofre preempção */
                historyType[processIdx] = pid;
                historyTime[processIdx] = quantumAtual;
                processIdx++;
                
                high ? pop(highPriority) : pop(lowPriority);
                currentProcess->next = NULL;

                push(lowPriority, currentProcess);

                changed = 3;
                changedID = pid;

                quantumAtual=0;
            }
        }

        if(changed>0){ /* se houve troca de contexto, vamos buscar o processo novo para ser executado */
            printf("Houve troca de processos na CPU!\n\n");
            printf("Processo %d saiu, pois ", changedID);

            if(changed==1){
                printf("I/O começou.");
            }else if(changed==2){
                printf("foi finalizado.");
            }else{
                printf("atingiu o limite do quantum.");
            }

            printf("\n\n");

            if(!isEmpty(highPriority)){ /* pego o novo processo da fila de alta prioridade, se tem alguém lá */
                high = 1;
                currentProcess = highPriority->front;
            }else if (!isEmpty(lowPriority)){  /* caso contrário, pego da baixa */
                high = 0;
                currentProcess = lowPriority->front;
            }
            else{ /* se as filas de alta e baixa estão vazias */
                currentProcess = NULL;
            }

            if(currentProcess==NULL){
                printf("Nenhum processo entrou na CPU!\n\n");
            }else{
                printf("Processo %d entrou na CPU!\n\n", currentProcess->pid);
            }

            if(displayQueues(currentTime, quantumAtual, currentProcess, highPriority, lowPriority, diskIOqueue, magneticTapeIOqueue, printerIOqueue) == 4 && completedProcess == N){  /* se todas as filas estão vazias, termina o escalonamento */
                printf("Escalonamento encerrado!\n\n");

                displayPlot(historyTime, historyType, processIdx);

                displayTurnarounds(N, turnaround);

                return;
            }
        }
        
        printf("\n\nEnter para continuar >>");
        fflush(stdin);
        getc(stdin);

        /* Verificação das filas de I/O  */
        if(!isEmpty(diskIOqueue)){ 
            process* temp = diskIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){  /* se acabou o I/O */
                currentIO[temp->pid]++;
                pop(diskIOqueue);        /* tira esse processo da fila de I/O */
                temp->next = NULL;
                push(lowPriority, temp); /* bota na fila de baixa prioridade */
            }
        }

        if(!isEmpty(magneticTapeIOqueue)){  
            process* temp = magneticTapeIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){    /* se acabou o I/O */
                currentIO[temp->pid]++;
                pop(magneticTapeIOqueue);  /* tira esse processo da fila de I/O */
                temp->next = NULL;
                push(highPriority, temp);  /* bota na fila de alta prioridade */
            }
        }

        if(!isEmpty(printerIOqueue)){  
            process* temp = printerIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){   /* se acabou o I/O */
                currentIO[temp->pid]++;
                pop(printerIOqueue);      /* tira esse processo da fila de I/O */
                temp->next = NULL;
                push(highPriority, temp); /* bota na fila de alta prioridade */
            }
        }
        
        if(currentProcess==NULL){ /* se não há nenhum processo na CPU */
            if(!wasNull) lastNull = currentTime; /* se o processo anterior não era nulo, então marco o tempo em que a ociosidade começou */
            
            wasNull = 1;

        }else{
            quantumAtual++;

            currentProcess->remainingTime--;
        }
    }
}

int main(){

    scheduling();

    return 0;
}
