# Multilevel Feedback Priority Queue Scheduler
This project implements a Multilevel Feedback Priority Queue (MLFQ) Scheduler in C. 

## Introduction
The MLFQ scheduler is a type of CPU scheduling algorithm that employs multiple queues with different priority levels. Processes are dynamically moved between queues based on their behavior and requirements.

## Features
**Multiple Priority Levels:** Supports four priority levels.\
**Process States:** Manages processes in different states - READY, RUNNING, BLOCKED, and FINISHED.\
**Semaphores:** Uses semaphores for synchronization between processes.\
**Memory Management:** Simple memory management for storing process control blocks (PCBs) and program instructions.\

## Installation
To compile and run the MLFQ scheduler, follow these steps:

### Clone the repository:

` git clone "https://github.com/georgebaher/C--Multilevel-Feedback-Priority-Queue-Scheduler" 
`
### Compile the scheduler:

` gcc -o mlfq_scheduler scheduler.c 
`
### Run the scheduler:

`./mlfq_scheduler
`

## Usage
The program will prompt you to enter the arrival times for three programs. It will then simulate the scheduling process, displaying the state of the system at each clock cycle.

## Code Structure
The code is organized into the following main components:

### Structures:

**PCB:** Represents a Process Control Block.\
**Queue:** Implements a basic queue.\
**MLQ:** Represents the multilevel queues.\
**MemoryWord:** Represents a word in the main memory.\
**Semaphore:** Represents a Mutex.\

### Functions:

*enqueue, dequeue, display_queue, dequeue_pid:* Queue operations.\
*print_pcb, readFromFile, get_highest_priority_process:* Utility functions.\
*change_pcb_priority, change_pcb_state, change_pcb_pc:* PCB management.\
*initializeSemaphores, _sem_wait, _sem_signal:* Semaphore operations.\
*parse_program, execute_instruction, mlfq_sched_exec:* Scheduler operations.\
*print_ready_queues, print_blocked_queues, print_clk_cycle, print_available_processes:* Debugging and display functions.

## How It Works
**Initialization:** The user is prompted to enter the arrival times for three programs.\
**Clock Cycle:** The scheduler runs in clock cycles, executing instructions from the highest priority queue.\
**Process Execution:** Each process is executed for a certain number of cycles based on its priority.\
**Semaphore Management:** Processes can wait for and signal semaphores for synchronization.\
**Priority Adjustment:** Processes are moved between queues based on their behavior and priority.\
**Termination:** The scheduler terminates when all queues are empty and no processes are left to execute.
