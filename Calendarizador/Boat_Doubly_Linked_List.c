#include <stdio.h>
#include <stdlib.h>
#include "types.h"


Boat_Doubly_Linked_List_t* create_boat_list(Boat_Doubly_Linked_List_t* list)
{
    list = malloc(sizeof(Boat_Doubly_Linked_List_t));
    list->first=NULL;
    return list;
}
int is_boat_list_empty(Boat_Doubly_Linked_List_t* list)
{
    if(list->first==NULL)
    {
        return 1;
    }
    return 0;
}
/* Given a reference (pointer to pointer) to the head of a
   list and an int, inserts a new node on the front of the
   list. */
void insert_first(Boat_Doubly_Linked_List_t* list, Boat_t* new_data)
{
    /* 1. allocate node */
    Boat_Doubly_Linked_List_Node_t* new_node = malloc(sizeof(Boat_Doubly_Linked_List_Node_t));

    /* 2. put in the data  */
    new_node->data = new_data;
    //printf("\nNew Node id: %d\n", new_node->data->id);

    /* 3. Make next of new node as head and previous as NULL
     */
    new_node->next = list->first;
    new_node->prev = NULL;

    /* 4. change prev of head node to new node */
    if(list->first != NULL)
    {
        list->first->prev = new_node;
    }

    /* 5. move the head to point to the new node */
    list->first = new_node;
}

/* Given a node as prev_node, insert a new node after the
 * given node */
void insertAfter(Boat_Doubly_Linked_List_Node_t* prev_node, Boat_t* new_data)
{
    /*1. check if the given prev_node is NULL */
    if(prev_node == NULL)
    {
        printf("the given previous node cannot be NULL");
        return;
    }

    /* 2. allocate new node */
    Boat_Doubly_Linked_List_Node_t* new_node = malloc(sizeof(Boat_Doubly_Linked_List_Node_t));

    /* 3. put in the data  */
    new_node->data = new_data;

    /* 4. Make next of new node as next of prev_node */
    new_node->next = prev_node->next;

    /* 5. Make the next of prev_node as new_node */
    prev_node->next = new_node;

    /* 6. Make prev_node as previous of new_node */
    new_node->prev = prev_node;

    /* 7. Change previous of new_node's next node */
    if(new_node->next != NULL)
    {
        new_node->next->prev = new_node;
    }
}

/* Given a reference (pointer to pointer) to the head
   of a DLL and an int, appends a new node at the end  */
void append_boat(Boat_Doubly_Linked_List_t* list, Boat_t* new_data)
{
    /* 1. allocate node */
    Boat_Doubly_Linked_List_Node_t* new_node = malloc(sizeof(Boat_Doubly_Linked_List_Node_t));

    Boat_Doubly_Linked_List_Node_t* last = list->first; /* used in step 5*/

    /* 2. put in the data  */
    new_node->data = new_data;

    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;

    /* 4. If the Linked List is empty, then make the new
          node as head */
    if(list->first == NULL)
    {
        new_node->prev = NULL;
        list->first = new_node;
        return;
    }

    /* 5. Else traverse till the last node */
    while(last->next != NULL)
    {
        last = last->next;
    }

    /* 6. Change the next of last node */
    last->next = new_node;

    /* 7. Make last node as previous of new node */
    new_node->prev = last;
}
void delete_first(Boat_Doubly_Linked_List_t* list)
{
    if(list->first!=NULL)
    {
        Boat_Doubly_Linked_List_Node_t* temp = list->first;
        list->first=list->first->next;
        free(temp);
    }
}
/* function to swap data of two nodes a and b*/
void swap(Boat_Doubly_Linked_List_Node_t* a, Boat_Doubly_Linked_List_Node_t* b)
{
    Boat_t* temp = a->data;
    a->data = b->data;
    b->data = temp;
}
void printBoat(Boat_t* boat)
{
    printf("Boat: %d", boat->id);
    printf("\tType: %d", boat->type);
    printf("\tPosition: %d", boat->position);
    printf("\tDirection: %d", boat->direction);
    printf("\tSpeed: %f", boat->speed);
    printf("\tLife time: %d\n", boat->life_time);
}
// This function prints contents of linked list starting
// from the given node
void printListDoubly(Boat_Doubly_Linked_List_t* list)
{
    if(list->first==NULL)
    {
        printf("\nEmpty list\n");
    }
    else
    {
        Boat_Doubly_Linked_List_Node_t* piv;
        Boat_Doubly_Linked_List_Node_t* piv2;
        piv=list->first;
        piv2=list->first;
        printf("\nTraversal in forward direction\n");
        while(piv != NULL)
        {
            if(piv2->next!=NULL)
            {
                piv2 = piv2->next;
            }
            printf(" %d ", piv->data->id);
            piv = piv->next;
        }
        printf("\nPrev id: %d\n", piv2->data->id);
        printf("\nTraversal in reverse direction \n");
        while(piv2 != NULL)
        {
            printf(" %d ", piv2->data->id);
            piv2 = piv2->prev;
        }
    }
}
void print_list(Boat_Doubly_Linked_List_t* list)
{
    if(list->first==NULL)
    {
        printf("Empty list\n");
    }
    else
    {
        Boat_Doubly_Linked_List_Node_t* piv;
        piv=list->first;
        while(piv != NULL)
        {
            printf("<-(%d)->", piv->data->id);
            piv = piv->next;
        }
        printf("\n");
    }
}
void print_list_speed(Boat_Doubly_Linked_List_t* list)
{
    if(list->first==NULL)
    {
        printf("Empty list\n");
    }
    else
    {
        Boat_Doubly_Linked_List_Node_t* piv;
        piv=list->first;
        //printf("\nTraversal in forward direction\n");
        while(piv != NULL)
        {
            printf("<-(");
            printf("%d, %d", piv->data->id, (int)piv->data->speed);
            printf(")->");
            piv = piv->next;
        }

    }
}
int get_length(Boat_Doubly_Linked_List_t* list)
{
    int length=0;
    if(list->first!=NULL)
    {
        Boat_Doubly_Linked_List_Node_t* piv;
        piv=list->first;
        //printf("\nTraversal in forward direction\n");
        while(piv != NULL)
        {
            piv = piv->next;
            length++;
        }
    }
    return length;
}
