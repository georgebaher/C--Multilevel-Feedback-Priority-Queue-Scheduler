#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#define QUEUE_MAX_SIZE 3
#define RELEASE_TIME_P1 0
#define RELEASE_TIME_P2 0
#define RELEASE_TIME_P3 0


struct mlq{
    Queue q1;
    Queue q2;
    Queue q3;
};

typedef struct {
    int pid;
    int priority;
    int state; //0 blocked, 1 ready, 2 running
    int pc;
    int mem_start;
    int mem_end;
}PCB;

typedef struct{
    int front,rear;
    PCB queue[QUEUE_MAX_SIZE];
    int count;
}Queue;

typedef struct{
    char name[20];
    char data[20];
}MemoryWord;

//function prototypes
void enqueue(Queue *q, PCB pcb);
PCB dequeue(Queue* q);
void display_queue(Queue q);

//global variables
MemoryWord main_memory[60];
PCB ready_queue[3];
struct mlq mlq;
int clock_cycle = 0;

void increment_clk(){
    if(clock_cycle==RELEASE_TIME_P1){
        //create process and allocate in memory and return pcb
        //enqueue the pcb in the highest priority ready queue
        //enqueue(&mlq.q1,pcb);
    }
    if(clock_cycle==RELEASE_TIME_P2)
        //
    if(clock_cycle==RELEASE_TIME_P3)
        //

    clock_cycle++;

}
void run(PCB p){
    
}
void mlfq_sched_exec(){
    if(mlq.q1.count>0){
        PCB p = dequeue(&mlq.q1);
        // run process p
        increment_clk();
        // not blocked
        if(p.state!=0){
            enqueue(&mlq.q2,p);
            p.priority++;
        }
    }
    else if(mlq.q2.count>0){
        PCB p = dequeue(&mlq.q2);
        for(int i=0;i<2;i++){
            //run process p
            increment_clk();
            if(p.state==0){
                break;
            }
        }
        if(p.state!=0){
            enqueue(&mlq.q3,p);
            p.priority++;
        }
    }
    else if(mlq.q3.count>0){
        PCB p = dequeue(&mlq.q3);
        for(int i=0;i<4;i++){
            //run process p
            increment_clk();
            if(p.state==0){
                break;
            }
        }
        if(p.state!=0){
            enqueue(&mlq.q3,p);
            p.priority++;
        }
    }
    else{
        printf("All queues are empty\n");
        //flag false->break in main()
    }

}

int main(){
    // parse programs 1,2 and 3
    // allocate memory and create PCBs for each program
    // put them in the ready queue in the correct order 
    // run the scheduler
    
    PCB p1,p2,p3,p4;
    p1.pid=1;
    p2.pid=2;
    p3.pid=3;
    p4.pid=4;
    Queue q;
    q.count=0;
    enqueue(&q,p1);
    enqueue(&q,p2);
    enqueue(&q,p3);
    display_queue(q);
    enqueue(&q,p4);
    dequeue(&q);
    display_queue(q);
    dequeue(&q);
    display_queue(q);
    dequeue(&q);
    display_queue(q);
    enqueue(&q,p1);
    display_queue(q);
    enqueue(&q,p2);
    display_queue(q);
    enqueue(&q,p3);
    display_queue(q);
    enqueue(&q,p4);
    dequeue(&q);
    display_queue(q);
    enqueue(&q,p4);
    display_queue(q);

   
   return 0;
}

void enqueue(Queue* q, PCB pcb){
    if((*q).count==QUEUE_MAX_SIZE){
        printf("Queue is full\n");
        return;
    }

     if ((*q).front == -1)
    {
        (*q).front = 0;
    }
    (*q).rear++;
    if ((*q).rear == QUEUE_MAX_SIZE )
    {
        (*q).rear=0;
    }
    (*q).queue[(*q).rear] = pcb;
    (*q).count++;

}

PCB dequeue(Queue* q)
{
    if ((*q).count==0)
    {
        printf("Queue is empty!\n");
        
    }
    else{
    PCB element = (*q).queue[(*q).front];
    (*q).front++;
    if((*q).front==QUEUE_MAX_SIZE){
        (*q).front=0;
    }
    (*q).count--;
    return element;
    }
}

void display_queue(Queue q){
    if(q.count==0){
        printf("Queue is empty\n");
        return;
    }
    bool exit=0;
    while(exit==0){
        if(q.front==q.rear)
            exit=1;
        printf("%d ", q.queue[q.front++].pid);
        if(q.front==QUEUE_MAX_SIZE)
          q.front=0;
    }
    printf("\n");
}


