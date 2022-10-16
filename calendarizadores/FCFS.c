// Source: https://www.edureka.co/blog/first-come-first-serve-scheduling/

#include<stdio.h>

int main() {
    
    int numberProcesses, i, j;
    
    printf(" Enter total number of processes:\t");
    scanf("%d", &numberProcesses);

    int duration[numberProcesses], waitingTime[numberProcesses], turnaroundTime[numberProcesses];
    
    printf("\n Enter Process Duration\n");
    
    for(i = 0; i < numberProcesses; i++) {
        
        printf(" Process[%d]: ", i + 1);
        scanf("%d", &duration[i]);
        
    }
    
    waitingTime[0] = 0;   
    
    for(i = 1; i < numberProcesses; i++) {
        
        waitingTime[i] = 0;
        
        for(j = 0; j < i; j++) {
            
            waitingTime[i] += duration[j];

		}
            
    }
    
    printf("\n Process No \t\t Duration \t\t Waiting Time \t\t Turnaround Time");
    
    for(i = 0; i < numberProcesses; i++) {
        
        turnaroundTime[i] = duration[i] + waitingTime[i];
        
        printf("\n Process No[%d] \t\t %d\t\t\t %d\t\t\t %d", i + 1, duration[i], waitingTime[i], turnaroundTime[i]);
        
    }
    
    printf("\n\n");

    return 0;
    
}

