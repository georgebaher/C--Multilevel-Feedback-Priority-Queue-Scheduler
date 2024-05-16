#define MAX_SIZE 3
#include <stdio.h>
#include<stdbool.h>
int queue[MAX_SIZE];
int front = -1;
int rear = -1;
int count=0;

void enqueue(int element)
{
    if(count==MAX_SIZE){
        printf("Cannot enqueue because queue is full!\n");
        return;
    }
    if (front == -1)
    {
        front = 0;
    }
    rear++;
    if (rear == MAX_SIZE )
    {
        rear=0;
    }
    queue[rear] = element;
    count++;
}


int dequeue()
{
    int finished=front==rear;
   
    if (count==0)
    {
        printf("Cannot dequeue because queue is empty!\n");
        return -1;
    }
    int element = queue[front];
    front++;
    if(front==MAX_SIZE){
        front=0;
    }
    count--;
    return element;
}

void display()
{
    // //print array
    // if (front == -1)
    // {
    //     printf("Queue is empty\n");
    //     return;
    // }
    // for(int i=0;i<MAX_SIZE;i++){
    //     printf("%d ",queue[i]);
    // }
    // printf("\n");
    // printf("front: %d\n",front);
    // printf("rear: %d\n",rear);
    // printf("\n");
    if(count==0){
        printf("Queue is empty\n");
        return;
    }
    int _front=front;
    bool exit=0;
    while(exit==0){
         if(_front==rear)
            exit=1;
        printf("%d ", queue[_front++]);
        if(_front==MAX_SIZE)
         _front=0;
    }
    printf("\n");
}

int main()
{
    enqueue(10);
    enqueue(20);
    enqueue(30);
    display(); //10 20 30
    dequeue(); //rem 10
    display(); //20 30
    enqueue(40); 
    display();//20 30 40
    enqueue(50);
    display();
    dequeue(); //rem 20
    display(); //30 40
    enqueue(50);
    display();//30 40 50
    dequeue(); //rem 30
    display(); //40 50
    dequeue(); //rem 40
    display(); //50
    dequeue(); //queue is empty
    display();
    dequeue();//cannot dequeue
    enqueue(60); //60
    display();
    enqueue(70); //60 70
    display();
    dequeue(); //70
    display();
    dequeue(); //queue is empty
    display(); 

    return 0;
}