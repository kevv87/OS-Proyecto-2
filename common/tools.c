//
// Created by Kevin Zeledón on 14/10/22.
//

#include <string.h>
#include <errno.h>

#include "include/tools.h"

void printError(char *message){
    printf("%s: %s\n", message, strerror(errno));
}

void printBuffer(char * buffer, int len){
    int i =0;
    for (i=0; i<len; i++) {
        printf("%02x ", buffer[i]);
        if ((i+1)%16 == 0) printf("\n");
    }
}