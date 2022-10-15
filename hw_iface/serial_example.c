#include <stdio.h>

#include "arduino-serial/arduino-serial-lib.h"
#include "common/include/tools.h"

int main() {
    int fd, rc;
    char *serialport = "/dev/cu.usbmodem1423101";
    int baudrate = 9600;

    fd = serialport_init(serialport, baudrate);

    if (fd < 0){
        printError("serial port initialization failed");
        return fd;
    }

    printf("successfully opened serialport %s @ %d bps\n", serialport, baudrate);
    serialport_flush(fd);

    struct mg_context *ctx;

    uint8_t message_byte = 10;
    rc = serialport_writebyte(fd, message_byte);

    if(rc < 0){
        printError("Writing to serial port failed");
        return rc;
    }

    serialport_close(fd);

    return 0;
}