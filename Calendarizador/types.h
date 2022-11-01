#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <semaphore.h>


typedef struct Boat
{
    int id;
    int type; // 0, 1, or 2
    int position; // index
    int direction; // 0 or 1
    double speed;
    int priority;
    int deadline;
}Boat_t;


//Canal node
typedef struct Canal_node
{
    sem_t  sem;
	Boat_t* boat;
}Canal_node_t;


//Boat Doubly Linked List node
typedef struct Boat_Doubly_Linked_List_Node Boat_Doubly_Linked_List_Node_t;
typedef struct Boat_Doubly_Linked_List_Node
{
    Boat_t* data;
    Boat_Doubly_Linked_List_Node_t* next;
    Boat_Doubly_Linked_List_Node_t* prev;
}Boat_Doubly_Linked_List_Node_t;

//Boat Doubly Linked List
typedef struct Boat_Doubly_Linked_List
{
    Boat_Doubly_Linked_List_Node_t *first;
}Boat_Doubly_Linked_List_t;


//Thread Doubly Linked List node
typedef struct Thread_Doubly_Linked_List_Node Thread_Doubly_Linked_List_Node_t;
typedef struct Thread_Doubly_Linked_List_Node
{
    pthread_t data;
    Thread_Doubly_Linked_List_Node_t* next;
    Thread_Doubly_Linked_List_Node_t* prev;
}Thread_Doubly_Linked_List_Node_t;

//Thread Doubly Linked List
typedef struct Thread_Doubly_Linked_List
{
    Thread_Doubly_Linked_List_Node_t *first;
}Thread_Doubly_Linked_List_t;


//Initial Load
typedef struct Load
{
    int left[3];
    int right[3];
}Load_t;


//Equity arguments
typedef struct Equity_arguments
{
    int w;
    int* finished_boats;
    sem_t* sem_finished_boats;
    Boat_t* boat;
}Equity_arguments_t;

//Equity arguments launcher
typedef struct Equity_arguments_launcher
{
    int direction;
    int w;
    int* finished_boats;
    sem_t* sem_finished_boats;
}Equity_arguments_launcher_t;


//Sign arguments
typedef struct Sign_arguments
{
    int* change_sign_flag;
    Boat_t* boat;
    sem_t* sem_change_sign_flag;
}Sign_arguments_t;

//Sign arguments launcher
typedef struct Sign_arguments_launcher
{
    int direction;
    int* change_sign_flag;
    sem_t* sem_change_sign_flag;
}Sign_arguments_launcher_t;


//Tico arguments
typedef struct Tico_arguments
{
    Boat_t* boat;
}Tico_arguments_t;

//Sign arguments launcher
typedef struct Tico_arguments_launcher
{
    int direction;
}Tico_arguments_launcher_t;


#endif // TYPES_H