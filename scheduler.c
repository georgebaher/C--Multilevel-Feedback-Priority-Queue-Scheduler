#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#define QUEUE_MAX_SIZE 3

typedef struct
{
    int pid;
    int priority;
    enum
    {
        READY,
        RUNNING,
        BLOCKED,
        FINISHED
    } state;
    int pc;
    int mem_start;
    int mem_end;
} PCB;

typedef struct
{
    int front, rear;
    int queue[QUEUE_MAX_SIZE];
    int count;
} Queue;

struct MLQ
{
    Queue q1;
    Queue q2;
    Queue q3;
    Queue q4;
};

typedef struct
{
    char name[25];
    char data[25];
} MemoryWord;

typedef struct
{
    char resource[25];
    bool busy;
    Queue blocked_queue;
} Semaphore;

Queue general_blocked_queue = {.front = -1, .rear = -1, .count = 0};

void printMainMemory();
void enqueue(Queue *q, int pid);
int dequeue(Queue *q);
void display_queue(Queue q);
void dequeue_pid(Queue *q, int pid);
void print_pcb(PCB p);
char *readFromFile(char *filename);
int get_highest_priority_process(Queue *q);

// global variables
int RELEASE_TIME_P1, RELEASE_TIME_P2, RELEASE_TIME_P3;
MemoryWord main_memory[60];
struct MLQ MLQ = {
    {.front = -1, .rear = -1, .count = 0},
    {.front = -1, .rear = -1, .count = 0},
    {.front = -1, .rear = -1, .count = 0},
    {.front = -1, .rear = -1, .count = 0}};
int clock_cycle = -1;
int ctr_main_memory_instructions = 0;
int ctr_main_memory_pcb = 59;
int ctr_pid = 1;
Semaphore userInput, userOutput, file;
bool TERMINATE = false;

PCB get_pcb_by_pid(int pid)
{
    PCB pcb;
    for (int i = 0; i < 60; i++)
    {
        if (strcmp(main_memory[i].name, "pid") == 0 && atoi(main_memory[i].data) == pid)
        {
            pcb.pid = atoi(main_memory[i].data);
            pcb.priority = atoi(main_memory[i - 1].data);
            pcb.state = atoi(main_memory[i - 2].data);
            pcb.pc = atoi(main_memory[i - 3].data);
            pcb.mem_start = atoi(main_memory[i - 4].data);
            pcb.mem_end = atoi(main_memory[i - 5].data);
            return pcb;
        }
    }
}

void change_pcb_priority(int pid, int new_priority)
{
    for (int i = 0; i < 60; i++)
    {
        if (strcmp(main_memory[i].name, "pid") == 0 && atoi(main_memory[i].data) == pid)
        {
            sprintf(main_memory[i - 1].data, "%d", new_priority);
            break;
        }
    }
}

void change_pcb_state(int pid, int new_state)
{
    for (int i = 0; i < 60; i++)
    {
        if (strcmp(main_memory[i].name, "pid") == 0 && atoi(main_memory[i].data) == pid)
        {
            sprintf(main_memory[i - 2].data, "%d", new_state);
            break;
        }
    }
}

void change_pcb_pc(int pid, int new_pc)
{
    for (int i = 0; i < 60; i++)
    {
        if (strcmp(main_memory[i].name, "pid") == 0 && atoi(main_memory[i].data) == pid)
        {
            sprintf(main_memory[i - 3].data, "%d", new_pc);
            break;
        }
    }
}

void initializeSemaphores()
{
    strcpy(userInput.resource, "userInput");
    userInput.busy = 0;
    userInput.blocked_queue.front = -1;
    userInput.blocked_queue.rear = -1;
    userInput.blocked_queue.count = 0;

    strcpy(userOutput.resource, "userOutput");
    userOutput.busy = 0;
    userOutput.blocked_queue.front = -1;
    userOutput.blocked_queue.rear = -1;
    userOutput.blocked_queue.count = 0;

    strcpy(file.resource, "file");
    file.busy = 0;
    file.blocked_queue.front = -1;
    file.blocked_queue.rear = -1;
    file.blocked_queue.count = 0;
}

void _sem_wait(Semaphore *s, int pid)
{
    if ((*s).busy)
    {
        // set the process state to blocked
        change_pcb_state(pid, BLOCKED);
        // enqueue the process in the general blocked queue
        enqueue((&general_blocked_queue), pid);
        // enqueue the process in the semaphore's blocked queue
        enqueue(&(*s).blocked_queue, pid);
    }
    else
    {
        (*s).busy = true;
    }
}

void _sem_signal(Semaphore *s)
{
    
    if ((*s).blocked_queue.count > 0)
    {
        int pid = get_highest_priority_process(&(*s).blocked_queue);
        // remove from general blocked queue
        dequeue_pid(&general_blocked_queue, pid);
        // remove from semaphore blocked queue
        dequeue_pid(&(*s).blocked_queue, pid);
        // change the state of the process to ready
        change_pcb_state(pid, READY);
        // enqueue the process in the ready queue according to its priority
        PCB pcb = get_pcb_by_pid(pid);
        switch (pcb.priority)
        {
        case 1:
            enqueue(&MLQ.q1, pid);
            break;
        case 2:
            enqueue(&MLQ.q2, pid);
            break;
        case 3:
            enqueue(&MLQ.q3, pid);
            break;
        case 4:
            enqueue(&MLQ.q4, pid);
            break;
        }
        (*s).busy = true;
    }
    else{
        (*s).busy = false;
    }
}

int parse_program(char *filename)
{
    FILE *fp;
    char line[256];
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return -1;
    }
    int old_ctr_main_memory = ctr_main_memory_instructions;
    int inst_cnt = 0;
    while (fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, "\n");
        if (token == NULL)
            continue; // Skip empty lines
        MemoryWord mw;
        sprintf(mw.name, "%s%d", "inst_", inst_cnt++);
        strcpy(mw.data, token);
        main_memory[ctr_main_memory_instructions++] = mw;
    }
    fclose(fp);
    // leave 3 empty spaces in memory for 3 variables
    for (int i = 0; i < 3; i++)
    {
        strcpy(main_memory[ctr_main_memory_instructions].name, "#");
        strcpy(main_memory[ctr_main_memory_instructions++].data, "_");
    }
    // create pcb for the program
    PCB pcb;
    pcb.pid = ctr_pid++;
    pcb.priority = 1;
    pcb.state = READY;
    pcb.pc = 0;
    pcb.mem_start = old_ctr_main_memory;
    pcb.mem_end = ctr_main_memory_instructions - 1;
    // store pcb in memory at the end
    strcpy(main_memory[ctr_main_memory_pcb].name, "pid");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.pid);
    strcpy(main_memory[--ctr_main_memory_pcb].name, "priority");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.priority);
    strcpy(main_memory[--ctr_main_memory_pcb].name, "state");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.state);
    strcpy(main_memory[--ctr_main_memory_pcb].name, "pc");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.pc);
    strcpy(main_memory[--ctr_main_memory_pcb].name, "mem_start");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.mem_start);
    strcpy(main_memory[--ctr_main_memory_pcb].name, "mem_end");
    sprintf(main_memory[ctr_main_memory_pcb].data, "%d", pcb.mem_end);
    ctr_main_memory_pcb--;

    return pcb.pid;
}

void increment_clk()
{
    clock_cycle++;
    if (clock_cycle == RELEASE_TIME_P1)
    {
        int pid = parse_program("Program_1.txt");
        // enqueue pcb in ready queue
        enqueue(&MLQ.q1, pid);
    }
    if (clock_cycle == RELEASE_TIME_P2)
    {
        int pid = parse_program("Program_2.txt");
        // enqueue pcb in ready queue
        enqueue(&MLQ.q1, pid);
    }
    if (clock_cycle == RELEASE_TIME_P3)
    {
        int pid = parse_program("Program_3.txt");
        // enqueue pcb in ready queue
        enqueue(&MLQ.q1, pid);
    }

    printf("----------Clock cycle<%d>----------\n", clock_cycle);
    printf("Q1: ");
    display_queue(MLQ.q1);
    printf("Q2: ");
    display_queue(MLQ.q2);
    printf("Q3: ");
    display_queue(MLQ.q3);
    printf("Q4: ");
    display_queue(MLQ.q4);

    printf("\ngeneral_blocked_queue: ");
    display_queue(general_blocked_queue);
    // print semaphore blocked queues
    printf("userInput_blocked_queue: ");
    display_queue(userInput.blocked_queue);
    printf("userOutput_blocked_queue: ");
    display_queue(userOutput.blocked_queue);
    printf("file_blocked_queue: ");
    display_queue(file.blocked_queue);
    // print state of each process pid 1 2 3
    printf("\npid 1 {");
    printf("state: %d, priority: %d}\n", get_pcb_by_pid(1).state, get_pcb_by_pid(1).priority);
    if (clock_cycle >= RELEASE_TIME_P2)
    {
        printf("pid 2 {");
        printf("state: %d, priority: %d}\n", get_pcb_by_pid(2).state, get_pcb_by_pid(2).priority);
    }
    if (clock_cycle >= RELEASE_TIME_P3)
    {
        printf("pid 3 {");
        printf("state: %d, priority: %d}\n", get_pcb_by_pid(3).state, get_pcb_by_pid(3).priority);
    }
    printf("-----------------------------------\n");
}

// function that returns the semaphore based on the resource name
Semaphore *getSemaphoreByName(char *name)
{
    // List of all semaphores
    Semaphore *semaphores[] = {&userInput, &userOutput, &file};
    Semaphore *semaphore = NULL;
    int numSemaphores = 3;
    // Iterate over all semaphores
    for (int i = 0; i < numSemaphores; i++)
    {
        // Check if the resource name matches the requested one
        if (strcmp((*semaphores[i]).resource, name) == 0)
        {
            // Return a pointer to the semaphore
            semaphore = semaphores[i];
        }
    }
    // No semaphore with the requested name was found
    return semaphore;
}

void execute_instruction(int pid)
{
    PCB p = get_pcb_by_pid(pid);
    if (p.state == READY)
        change_pcb_state(pid, RUNNING);
    // get instruction from memory
    char current_instruction[25];
    strcpy(current_instruction, main_memory[p.mem_start + p.pc].data);
    char *token = strtok(current_instruction, " ");

    if (strcmp(token, "semWait") == 0)
    {
        // get resource name (right of the first space)
        char *resource_name = strtok(NULL, " ");
        Semaphore *mySemaphore = getSemaphoreByName(resource_name);
        // check if the semaphore exists
        if (mySemaphore == NULL)
        {
            printf("Semaphore with name %s does not exist\n", resource_name);
            return;
        }
        // use the semaphore
        _sem_wait(mySemaphore, pid);
    }
    if (strcmp(token, "semSignal") == 0)
    {
        // get resource name (right of the first space)
        char *resource_name = strtok(NULL, " ");
        Semaphore *mySemaphore = getSemaphoreByName(resource_name);
        // check if the semaphore exists
        if (mySemaphore == NULL)
        {
            printf("Semaphore with name %s does not exist\n", resource_name);
            return;
        }
        // use the semaphore
        _sem_signal(mySemaphore);
    }
    if (strcmp(token, "assign") == 0)
    {
        int start = p.mem_end - 2;
        char *variable_name = strtok(NULL, " ");
        char *variable_value = strtok(NULL, " ");
        if (strcmp(variable_value, "readFile") == 0)
        {
            char *var_storing_filename = strtok(NULL, " ");
            char *filename;
            for (int i = start; i < start + 3; i++)
            {
                if (strcmp(main_memory[i].name, var_storing_filename) == 0)
                {
                    filename = main_memory[i].data;
                    break;
                }
            }
            variable_value = readFromFile(filename);
        }
        else
        {
            printf("P%d/ Enter value for %s: ", pid, variable_name);
            scanf("%s", variable_value);
        }

        for (int i = start; i < start + 3; i++)
        {
            if (strcmp(main_memory[i].name, "#") == 0)
            {
                strcpy(main_memory[i].name, variable_name);
                strcpy(main_memory[i].data, variable_value);
                break;
            }
        }
    }
    if (strcmp(token, "printFromTo") == 0)
    {
        char *a = strtok(NULL, " ");
        char *b = strtok(NULL, " ");
        int from;
        int to;

        int start = p.mem_end - 2;
        for (int i = start; i < start + 3; i++)
        {
            if (strcmp(main_memory[i].name, a) == 0)
            {
                from = atoi(main_memory[i].data);
            }
            if (strcmp(main_memory[i].name, b) == 0)
            {
                to = atoi(main_memory[i].data);
            }
        }
        printf("------------printFromTo------------\n");
        for (int i = from; i <= to; i++)
        {
            printf("%d\n", i);
        }
        printf("----------------------------------\n");
    }
    if (strcmp(token, "writeFile") == 0)
    {
        char *a = strtok(NULL, " ");
        char *b = strtok(NULL, " ");
        char *filename;
        char *data;
        int start = p.mem_end - 2;
        for (int i = start; i < start + 3; i++)
        {
            if (strcmp(main_memory[i].name, a) == 0)
            {
                filename = main_memory[i].data;
            }
            if (strcmp(main_memory[i].name, b) == 0)
            {
                data = main_memory[i].data;
            }
        }
        FILE *fp;
        fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf("Could not open file %s", filename);
            return;
        }
        fprintf(fp, "%s", data);
        fclose(fp);
    }
    if (strcmp(token, "print") == 0)
    {
        char *a = strtok(NULL, " ");
        char *data;
        int start = p.mem_end - 2;
        for (int i = start; i < start + 3; i++)
        {
            if (strcmp(main_memory[i].name, a) == 0)
            {
                data = main_memory[i].data;
            }
        }
        printf("---------------print---------------\n");
        printf("%s\n", data);
        printf("-----------------------------------\n");
    }

    // increment pc
    change_pcb_pc(pid, p.pc + 1);
    // check if the program has finished
    if (p.mem_start + p.pc == p.mem_end - 3)
        change_pcb_state(pid, FINISHED);
}

char *readFromFile(char *filename)
{
    FILE *fp;
    char line[256];
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return NULL;
    }
    char *data = (char *)malloc(256 * sizeof(char));
    while (fgets(line, sizeof(line), fp))
    {
        data = strtok(line, "\n");
    }
    fclose(fp);
    return data;
}

void mlfq_sched_exec()
{
    if (MLQ.q1.count > 0)
    {
        int pid = dequeue(&MLQ.q1);
        execute_instruction(pid);
        increment_clk();

        // not blocked
        if (get_pcb_by_pid(pid).state == BLOCKED || get_pcb_by_pid(pid).state == FINISHED)
        {
            return;
        }
        // change state to ready
        change_pcb_state(pid, READY);
        // increment priority
        change_pcb_priority(pid, 2);
        // enqueue in q2
        enqueue(&MLQ.q2, pid);
    }
    else if (MLQ.q2.count > 0)
    {
        int pid = dequeue(&MLQ.q2);
        for (int i = 0; i < 2; i++)
        {
            execute_instruction(pid);
            increment_clk();
            if (get_pcb_by_pid(pid).state == BLOCKED || get_pcb_by_pid(pid).state == FINISHED)
            {
                return;
            }
        }
        // change state to ready
        change_pcb_state(pid, READY);
        // increment priority
        change_pcb_priority(pid, 3);
        // enqueue in q3
        enqueue(&MLQ.q3, pid);
    }
    else if (MLQ.q3.count > 0)
    {
        int pid = dequeue(&MLQ.q3);
        for (int i = 0; i < 4; i++)
        {
            execute_instruction(pid);
            increment_clk();
            if (get_pcb_by_pid(pid).state == BLOCKED || get_pcb_by_pid(pid).state == FINISHED)
            {
                return;
            }
        }
        // change state to ready
        change_pcb_state(pid, READY);
        // increment priority
        change_pcb_priority(pid, 4);
        // enqueue in q4
        enqueue(&MLQ.q4, pid);
    }
    else if (MLQ.q4.count > 0)
    {
        int pid = dequeue(&MLQ.q4);
        for (int i = 0; i < 8; i++)
        {
            execute_instruction(pid);
            increment_clk();
            if (get_pcb_by_pid(pid).state == BLOCKED || get_pcb_by_pid(pid).state == FINISHED)
            {
                return;
            }
        }
        // change state to ready
        change_pcb_state(pid, READY);
        // enqueue in q4
        enqueue(&MLQ.q4, pid);
    }
    else
    {
        TERMINATE = true;
    }
}

int main()
{

    // get arrival time of each program from the user
    printf("Enter the arrival time of each program\n");
    printf("Program 1: ");
    scanf("%d", &RELEASE_TIME_P1);
    printf("Program 2: ");
    scanf("%d", &RELEASE_TIME_P2);
    printf("Program 3: ");
    scanf("%d", &RELEASE_TIME_P3);
    initializeSemaphores();
    Semaphore *s = getSemaphoreByName("userInput");
    increment_clk();
    while (!TERMINATE)
    {
        mlfq_sched_exec();
    }
    printf("            TERMINATED\n-----------------------------------\n");
    printMainMemory();
    return 0;
}
void print_pcb(PCB p)
{
    printf("-----PCB-----\n");
    printf("pid: %d\n", p.pid);
    printf("priority: %d\n", p.priority);
    printf("state: %d\n", p.state);
    printf("pc: %d\n", p.pc);
    printf("mem_start: %d\n", p.mem_start);
    printf("mem_end: %d\n", p.mem_end);
    printf("-------------\n");
}

void printMainMemory()
{
    printf("------------Main Memory------------\n");
    for (int i = 0; i < 60; i++)
    {
        printf("%s: %s\n", main_memory[i].name, main_memory[i].data);
    }
    printf("-----------------------------------\n");
}

void enqueue(Queue *q, int pid)
{
    if ((*q).count == QUEUE_MAX_SIZE)
    {
        printf("Queue is full\n");
        return;
    }

    if ((*q).front == -1)
    {
        (*q).front = 0;
    }
    (*q).rear++;
    if ((*q).rear == QUEUE_MAX_SIZE)
    {
        (*q).rear = 0;
    }
    (*q).queue[(*q).rear] = pid;
    (*q).count++;
}

int dequeue(Queue *q)
{
    int pid;
    if ((*q).count == 0)
    {
        printf("Queue is empty!\n");
        return -1;
    }
    else
    {
        pid = (*q).queue[(*q).front];
        (*q).front++;
        if ((*q).front == QUEUE_MAX_SIZE)
        {
            (*q).front = 0;
        }
        (*q).count--;
    }
    return pid;
}

void dequeue_pid(Queue *q, int pid)
{
    int count = (*q).count;
    for (int i = 0; i < count; i++)
    {
        int tmp = dequeue(q);
        if (tmp != pid)
        {
            enqueue(q, tmp);
        }
    }
}

void display_queue(Queue q)
{
    if (q.count == 0)
    {
        printf("Queue is empty\n");
        return;
    }
    bool exit = 0;
    while (exit == 0)
    {
        if (q.front == q.rear)
            exit = 1;
        printf("%d ", q.queue[q.front++]);
        if (q.front == QUEUE_MAX_SIZE)
            q.front = 0;
    }
    printf("\n");
}

int get_highest_priority_process(Queue *q)
{
    int highest_priority = 5;
    int highest_priority_pid = -1;
    for (int i = 0; i < (*q).count; i++)
    {
        int pid = dequeue(q);
        enqueue(q, pid);
        PCB pcb = get_pcb_by_pid(pid);
        if (pcb.priority < highest_priority)
        {
            highest_priority = pcb.priority;
            highest_priority_pid = pid;
        }
    }
    return highest_priority_pid;
}