// Source: https://www.sanfoundry.com/c-program-priority-scheduling/#:~:text=Priority%20Scheduling%20is%20a%20CPU,%3A%20Preemptive%20and%20Non%2DPreemptive.

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "Boat_Doubly_Linked_List.c"

void Priority(Boat_Doubly_Linked_List_t* list) {
    
    Boat_Doubly_Linked_List_Node_t* ptrCurrent;
    Boat_Doubly_Linked_List_Node_t* ptrAux = NULL;

    if(list -> first == NULL) {

        return;

    }

    ptrCurrent = list -> first;

    while (ptrCurrent -> next != ptrAux) {

        if (ptrCurrent -> data -> priority < ptrCurrent -> next -> data -> priority) {

            swap(ptrCurrent, ptrCurrent -> next);            

        }

        ptrCurrent = ptrCurrent -> next;

    }

    ptrAux = ptrCurrent;

}

/*
void swap(int *a,int *b) {
	
    int temp = *a;
    *a = *b;
    *b = temp;
    
}

int main() {
	
    int numberProcesses, i, j;
    
    printf(" Enter total number of processes:\t");
    scanf("%d", &numberProcesses);

    int duration[numberProcesses], priority[numberProcesses], index[numberProcesses];
    
    for(i = 0; i < numberProcesses; i++) {
        
        printf("\n Enter Details of Process[%d]:", i + 1);
        
        printf("\n Duration:\t\t");
		
		scanf("%d", &duration[i]);
		
		printf(" Priority Value:\t");
		
		scanf("%d", &priority[i]);          
        
        index[i] = i + 1;
        
    }
    
    for(i = 0; i < numberProcesses; i++) {
        
        int currentPriority = priority[i], currentNumberProcess = i;

        //Finding out highest priority element and placing it at its desired position
        for(j = i; j < numberProcesses; j++) {
            
            if(priority[j] > currentPriority) {
                
                currentPriority = priority[j];
                currentNumberProcess = j;
                
            }
            
        }

        //Swapping processes
        swap(&priority[i], &priority[currentNumberProcess]);
        swap(&duration[i], &duration[currentNumberProcess]);
        swap(&index[i], &index[currentNumberProcess]);
        
    }
    
    printf("\n Process No \t\t Duration \t\t Waiting Time \t\t Turnaround Time");
    
    int waitTime = 0;
    
    for(i = 0; i < numberProcesses; i++) {
            
        printf("\n Process No[%d] \t\t %d\t\t\t %d\t\t\t %d", index[i], duration[i], waitTime, waitTime + duration[i]);
        
        waitTime += duration[i];
        
    }

    printf("\n\n");
    
    return 0;
    
}
*/