//
// Created by Kevin Zeledón on 14/10/22.
//

#include <string.h>
#include <errno.h>

#include "include/tools.h"

void printError(char *message){
    printf("%s: %s", message, strerror(errno));
}