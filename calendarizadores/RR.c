// Source: https://www.edureka.co/blog/round-robin-scheduling-in-c/

#include <stdio.h>
//#include <string.h>
//#include "types.h"
//#include "Boat_Doubly_Linked_List.c"

int main() {
		
	int numberProcesses, numberProcessesAux, timeQuantum, i;
	int total = 0, counter = 0;
	
	printf(" Enter total number of processes:\t");
	scanf("%d", &numberProcesses);

	int arrivalTime[numberProcesses], duration[numberProcesses], temp[numberProcesses];
	
	numberProcessesAux = numberProcesses;
	
	for(i = 0; i < numberProcesses; i++) {
	
		printf("\n Enter Details of Process[%d]", i + 1);
		
		printf("\n Arrival Time:\t");
		
		scanf("%d", &arrivalTime[i]);
		
		printf(" Duration:\t");
		
		scanf("%d", &duration[i]);
		
		temp[i] = duration[i];
	
	}
	
	printf("\n Enter Time Quantum:\t");
	
	scanf("%d", &timeQuantum);
	
	printf("\n Process No \t\t Duration \t\t Waiting Time \t\t Turnaround Time");
	
	for(total = 0, i = 0; numberProcessesAux != 0;) {
	
		if(temp[i] <= timeQuantum && temp[i] > 0) {
		
			total = total + temp[i];
			temp[i] = 0;
			counter = 1;
		
		} else if(temp[i] > 0) {   	
			
			temp[i] = temp[i] - timeQuantum;
			total = total + timeQuantum;
			
		}
		
		if(temp[i] == 0 && counter == 1) {

			numberProcessesAux--;
			
			printf("\n Process No[%d] \t\t %d\t\t\t %d\t\t\t %d", i + 1, duration[i], total - arrivalTime[i] - duration[i], total - arrivalTime[i]);	

			counter = 0;
		
		}
		
		if(i == numberProcesses - 1) {
		
			i = 0;
		
		} else if(arrivalTime[i + 1] <= total) {
		
			i++;
		
		} else {
		
			i = 0;
		
		}
	
	}
	
	printf("\n\n");
	
	return 0;

}















/*
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





void RR(Boat_Doubly_Linked_List_t* list) {




}
*/























