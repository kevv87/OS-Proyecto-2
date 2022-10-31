#include <stdio.h>
#include <string.h>
#include "types.h"
#include "Boat_Doubly_Linked_List.c"

void RR(Boat_Doubly_Linked_List_t* list) {
    // Nothing
}

void Priority(Boat_Doubly_Linked_List_t* list) {

    int swapped;

    Boat_Doubly_Linked_List_Node_t* currentNode;
    Boat_Doubly_Linked_List_Node_t* auxNode = NULL;

    if(list -> first == NULL) {

        return;

    }

    do {

        swapped = 0;
        currentNode = list -> first;

        while(currentNode -> next != auxNode) {

            if(currentNode -> data -> priority > currentNode -> next -> data -> priority) {

                swap(currentNode, currentNode -> next);
                swapped = 1;

            }

            currentNode = currentNode -> next;

        }

        auxNode = currentNode;

    } while(swapped);

}

void SJF(Boat_Doubly_Linked_List_t* list) {

    int swapped;

    Boat_Doubly_Linked_List_Node_t* ptr1;
    Boat_Doubly_Linked_List_Node_t* lptr = NULL;

    /* Checking for empty list */
    if(list -> first == NULL) {

        return;

    }

    do {

        swapped = 0;
        ptr1 = list -> first;

        while(ptr1 -> next != lptr) {

            if(ptr1 -> data -> speed < ptr1 -> next -> data -> speed) {

                swap(ptr1, ptr1 -> next);
                swapped = 1;

            }

            ptr1 = ptr1 -> next;

        }

        lptr = ptr1;

    } while(swapped);

}

void FCFS(Boat_Doubly_Linked_List_t* list) {
    // Nothing
}

/*
int checkConditionEDF(Boat_Doubly_Linked_List_t* list) {
    int mu = 0;
    Boat_Doubly_Linked_List_Node_t* currentNode;
    Boat_Doubly_Linked_List_Node_t* auxNode = NULL;
    if(list -> first == NULL) {
        return;
    }

    currentNode = list -> first;
    while(currentNode -> next != auxNode) {
        mu++;
        currentNode = currentNode -> next;
    }
    if(mu <= 1) {
        return 1;
    } else {
        return 0;
    }
}
*/

void EDF(Boat_Doubly_Linked_List_t* list)
{

    int swapped;

    Boat_Doubly_Linked_List_Node_t* currentNode;
    Boat_Doubly_Linked_List_Node_t* auxNode = NULL;

    if(list -> first == NULL) {

        return;

    }

    /*
    int checked = checkConditionEDF(list);
    if(checked) {
    */

    do {

        swapped = 0;
        currentNode = list -> first;

        while (currentNode -> next != auxNode) {

            if (currentNode -> data -> deadline > currentNode -> next -> data -> deadline) {

                swap(currentNode, currentNode -> next);
                swapped = 1;

            }

            currentNode = currentNode -> next;

        }

        auxNode = currentNode;

    } while(swapped);

    /*
    } else {
        printf("No se cumple la condición µ ≤ 1 del algoritmo EDF\n");
    }
    */

}

void schedule(int calendarization_algorithm, Boat_Doubly_Linked_List_t* list) {

    switch(calendarization_algorithm) {

        case 0:
            // RR
            break;

        case 1:
            Priority(list);
            break;

        case 2:
            SJF(list);
            break;

        case 3:
            //FCFS
            break;

        case 4:
            EDF(list);
            break;

        default:
            printf("No se selecciono un metodo de control de flujo valido\n");

    }

}