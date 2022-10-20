#include <stdio.h>
#include <string.h>
#include "types.h"
#include "Boat_Doubly_Linked_List.c"


void FCFS(Boat_Doubly_Linked_List_t* list)
{
    //equis de
}

void SJF(Boat_Doubly_Linked_List_t* list)
{
    int swapped, i;
    Boat_Doubly_Linked_List_Node_t* ptr1;
    Boat_Doubly_Linked_List_Node_t* lptr = NULL;

    /* Checking for empty list */
    if(list->first == NULL)
    {
        return;
    }
    do
    {
        swapped = 0;
        ptr1 = list->first;

        while (ptr1->next != lptr)
        {
            if (ptr1->data->speed < ptr1->next->data->speed)
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while(swapped);
}
void schedule(int calendarization_algorithm, Boat_Doubly_Linked_List_t* list)
{
    switch(calendarization_algorithm)
    {
        case 0: break;//RR
        case 1: break;//Prioridad
        case 2: SJF(list); break;
        case 3: /*equis de*/ break;
        case 4: break; //RT
        default: printf("No se selecciono un metodo de control de flujo valido\n");
    }
}
