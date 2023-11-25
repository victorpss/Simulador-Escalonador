#include <stdio.h>
#include <stdlib.h>

typedef struct process{
    int pid; // ID do processo
    int arrivalTime; // Tempo de chegada
    int executeTime; // Tempo de execução
    int remainingTime; // Tempo restante
    int IOremainingTime; // Tempo restante da requisição IO
    int IOqnt;
    int* IOtime; // Tempo(s) de E/S
    int* IOtype; // Tipo(s) de E/S
    
    struct process *next;

} process;

typedef struct queue{
   process *front, *back;

} queue;

queue* initQueue(){
    queue* fila = (queue*) malloc(sizeof(queue)); //Aloca espaço na memória para a fila
    fila->front = fila->back = NULL; //Fila está vazia

    return fila;
}

process* initProcess(int pid, int arrivalTime, int executeTime, int IOtime[], int IOtype[], int IOqnt){
    
    
    // if(IOtime_size != IOtype_size){
    //     printf("ERRO PROCESSO %d - Tamanho do array [tempos das I/O] diferente do tamanho do array [tipos das I/O]", pid);
    //     return NULL;
    // }
    
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

        int i;
        for(i=0; i<IOqnt; i++){
            p->IOtime[i] = IOtime[i];
            p->IOtype[i] = IOtype[i];
        }
    }

    p->next = NULL;

    return p;
}

int isEmpty(queue* fila){
    return (fila->front==NULL);
}

int calculateIO(int num){
    switch(num){
        case 0:
            return 4;   //tempo do Disco
        case 1:
            return 8;   //tempo da fita
        case 2:
            return 12;  //tempo da impressora
        default:
            return 0;            
    }
}

void push(queue* fila, process* p){
    if(isEmpty(fila)){
        fila->front = fila->back = p;
        return;
    }

    fila->back->next = p;
    fila->back = p;
}

void pop(queue* fila){
    process* tmp = fila->front;
    fila->front = fila->front->next;

    if(fila->front==NULL){
        fila->back = NULL;
    }
}

int displayQueues(int currentTime, queue* highPriority, queue* lowPriority, queue* diskIOqueue, queue* magneticTapeIOqueue, queue* printerIOqueue){//Mostra as filas

    int queuesEmpty = 0;

    printf("Tempo: %d\n\n", currentTime);

    if(isEmpty(highPriority) && isEmpty(lowPriority)){ //Como são duas filas, verifica-se se as duas estão vazias em simultâneo
        printf("Fila da CPU está vazia!\n\n");
        queuesEmpty++; //Se a fila está vazia, soma ao contador de filas vazias
    }
    else{
        process* atual;

        if(!isEmpty(highPriority)){
            atual = highPriority->front;
        }else{
            atual = lowPriority->front;
        }

        printf("ID do processo atual da CPU: %d\n", atual->pid);
        printf("Tempo de serviço faltante: %d\n\n", atual->remainingTime);
    }

    if(!isEmpty(diskIOqueue)){
        process* atual = diskIOqueue->front;
        printf("ID do processo atual do Disco: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n\n", atual->IOremainingTime);
    }
    else{
        printf("Fila do Disco está vazia!\n\n");
        queuesEmpty++;
    }

    if(!isEmpty(magneticTapeIOqueue)){
        process* atual = magneticTapeIOqueue->front;
        printf("ID do processo atual da Fita magnética: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n\n", atual->IOremainingTime);

    }else{
        printf("Fila da Fita magnética está vazia!\n\n");
        queuesEmpty++;
    }

    if(!isEmpty(printerIOqueue)){
        process* atual = printerIOqueue->front;
        printf("ID do processo atual da Impressora: %d\n", atual->pid);
        printf("Tempo de I/O faltante: %d\n\n", atual->IOremainingTime);

    }else{
        printf("Fila da Impressora está vazia!\n\n");
        queuesEmpty++;
    }

    printf("\n\nEnter para continuar >>");
    fflush(stdin);
    getc(stdin);
    
    return queuesEmpty;
}

void displayPlot(int historyTime[], int historyType[], int processIdx){
    int i, j;

    for(i=0; i<processIdx; i++){
        for(j=0; j<historyTime[i]+1; j++){
            printf(" ");
        }
        if(historyType[i]!=-1) printf("%d", historyType[i]);
        else printf(" ");
        for(j=0; j<historyTime[i]+1; j++){
            printf(" ");
        }
        printf("  ");
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
        printf("  ");
    }
}

void scheduling(){
    int N = 3;

    // Novos processos
    queue* newProcess = initQueue();

    // Processos prontos
    queue* highPriority = initQueue();
    queue* lowPriority = initQueue();
    
    // Processos bloqueados
    queue* diskIOqueue = initQueue();
    queue* magneticTapeIOqueue = initQueue();
    queue* printerIOqueue = initQueue();

    int quantum = 3;
    int turnaround[N];
    
    int currentTime;
    int nextIOtime, nextIOtype;

    int currentIO[N];

    int processIdx=0, historyTime[1000], historyType[1000];
    
    int completedProcess = 0;
    
    int i;
    for(i=0; i<N; i++){
        currentIO[i]=0;
    }

    process* currentProcess;

    // processos precisam ser colocados em ordem crescente de "tempo de ativação" na fila de novos processos - Cabral, Pedro
    // IOtime contém as unidades de tempo do processo em execução onde há requisição I/O
    // Iotype contém os tipos dessas requisições (Disco - 0; Fita magnética - 1; Impressora - 2)

    int IOtime1[] = {2, 4};
    int IOtype1[] = {0, 1};

    process* p1 = initProcess(0, 0, 12, IOtime1, IOtype1, 2); //inicia processo 1
    push(newProcess, p1);

    int* IOtime2 = NULL;
    int* IOtype2 = NULL;

    process* p2 = initProcess(1, 2, 10, IOtime2, IOtype2, 0); //inicia processo 2
    push(newProcess, p2);

    int IOtime3[] = {5};
    int IOtype3[] = {2};

    process* p3 = initProcess(2, 4, 9, IOtime3, IOtype3, 1); //inicia processo 3
    push(newProcess, p3);
    
    int quantumAtual=0; //por quantas u.t. seguidas o processo tá sendo executado na cpu 
    int high; // 1 - estou executando alguem que veio da fila de alta; 0 - estou executando alguem que veio da baixa

    for(currentTime = 0; ; currentTime++){
        // Verificação da fila de novos
        #ifdef _WIN32
            system("cls");
        #else 
            system("clear");
        #endif
        
        while(!isEmpty(newProcess) && currentTime == newProcess->front->arrivalTime){  //se na unidade de tempo atual chegou algum processo (no critério de desempate, novos processos entram antes)
            process* temp = newProcess->front;
            pop(newProcess);  //tira esse processo da fila de novos
            temp->next = NULL; //tira a referência ao próximo processo a ficar pronto
            push(highPriority, temp);  //bota na fila de alta prioridade
        }

        if(displayQueues(currentTime, highPriority, lowPriority, diskIOqueue, magneticTapeIOqueue, printerIOqueue)==4 && completedProcess == N){  //se toda as filas estão vazias, termina o escalonamento
            printf("Escalonamento encerrado!\n\n");

            displayPlot(historyTime, historyType, processIdx);

            return;
        }

        // Verificação das filas de I/O 

        if(!isEmpty(diskIOqueue)){ 
            process* temp = diskIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){  //se acabou o I/O
                currentIO[temp->pid]++;
                pop(diskIOqueue);        //tira esse processo da fila de I/O
                temp->next = NULL;
                push(lowPriority, temp); //bota na fila de baixa prioridade
            }
        }

        if(!isEmpty(magneticTapeIOqueue)){  
            process* temp = magneticTapeIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){     //se acabou o I/O
                currentIO[temp->pid]++;
                pop(magneticTapeIOqueue);   //tira esse processo da fila de I/O
                temp->next = NULL;
                push(highPriority, temp);   //bota na fila de alta prioridade
            }
        }

        if(!isEmpty(printerIOqueue)){  
            process* temp = printerIOqueue->front;

            temp->IOremainingTime--;

            if(!temp->IOremainingTime){ //se acabou o I/O
                currentIO[temp->pid]++;
                pop(printerIOqueue);      //tira esse processo da fila de I/O
                temp->next = NULL;
                push(highPriority, temp); //bota na fila de alta prioridade
            }
        }

        if(!quantumAtual){ //se acabou a fatia de tempo de um processo (se houve troca de contexto), vamos buscar o processo novo para ser executado
            if(!isEmpty(highPriority)){ // pego o novo processo da fila de alta prioridade, se tem alguém lá
                high = 1;
                currentProcess = highPriority->front;
            }else if (!isEmpty(lowPriority)){  //else, pego da baixa
                high = 0;
                currentProcess = lowPriority->front;
            }
            else{ //se as filas de alta e baixa estão vazias
                currentProcess = NULL;
                continue;
            }
        }
        
        int pid = currentProcess->pid;

        if(currentIO[pid]<currentProcess->IOqnt){
            nextIOtype = currentProcess->IOtype[currentIO[pid]];    //tipo do próximo IO
            nextIOtime = currentProcess->IOtime[currentIO[pid]];    //próximo tempo do IO do processo atual
        }else{
            nextIOtime = -1;
        }

        if(currentProcess->executeTime - currentProcess->remainingTime == nextIOtime){  //se chegou o tempo do próximo IO
            
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

            quantumAtual=0;

            continue;
        }
        
        quantumAtual++;

        currentProcess->remainingTime--;

        if(!currentProcess->remainingTime){ // se um processo terminou a execução
            historyType[processIdx] = pid;
            historyTime[processIdx] = quantumAtual;
            processIdx++;

            high ? pop(highPriority) : pop(lowPriority);
            quantumAtual=0;
            turnaround[currentProcess->pid] = (currentTime + 1) - currentProcess->arrivalTime; //verificar se tá certo depois
            completedProcess++;
            continue;
        }

        if(quantumAtual == quantum){ // se esgotou a fatia de tempo, o processo sofre preempção
            historyType[processIdx] = pid;
            historyTime[processIdx] = quantumAtual;
            processIdx++;
            
            high ? pop(highPriority) : pop(lowPriority);
            currentProcess->next = NULL;
            push(lowPriority, currentProcess);
            quantumAtual=0;
            continue;
        }
    }
}

int main(){

    scheduling();

    return 0;
}