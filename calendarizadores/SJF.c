// Source: https://www.edureka.co/blog/sjf-scheduling-in-c/

#include<stdio.h>

int main() {

    int numberProcesses, pos, temp, i, j;
    int total = 0;
    
    printf(" Enter total number of processes:\t");
    scanf("%d",&numberProcesses);

    int duration[numberProcesses], processes[numberProcesses], waitingTime[numberProcesses], turnaroundTime[numberProcesses];

    printf("\n Enter Duration: \n");
    
    for(i = 0; i < numberProcesses; i++) {
        
        printf(" Process[%d]: ", i + 1);
        
        scanf("%d", &duration[i]);
        
        processes[i] = i + 1;         
        
    }

    for(i = 0; i < numberProcesses; i++) {
        
        pos = i;
        
        for(j = i + 1; j < numberProcesses; j++) {
            
            if(duration[j] < duration[pos]) {
                
                pos = j;

			}
            
        }

        temp = duration[i];
        duration[i] = duration[pos];
        duration[pos] = temp;

        temp = processes[i];
        processes[i] = processes[pos];
        processes[pos] = temp;
        
    }
    
    waitingTime[0] = 0;            
    
    for(i = 1; i < numberProcesses; i++) {
        
        waitingTime[i] = 0;
        
        for(j = 0; j < i; j++) {
            
            waitingTime[i] += duration[j];

		}
        
        total += waitingTime[i];
        
    }   

    total = 0;

    printf("\n Process No \t\t Duration \t\t Waiting Time \t\t Turnaround Time");
    
    for( i = 0; i < numberProcesses; i++) {
        
        turnaroundTime[i] = duration[i] + waitingTime[i];   
        total += turnaroundTime[i];
        
        printf("\n Process No[%d] \t\t %d\t\t\t %d\t\t\t %d", processes[i], duration[i], waitingTime[i], turnaroundTime[i]);
        
    }

    printf("\n\n");
    
    return 0;

}
