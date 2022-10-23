//
// Created by Kevin Zeledón on 15/10/22.
//

#include <string.h>
#include "include/hw_iface.h"

int initialize_serial_connection(IfaceContext *context){
    int fd;
    char *serialport = SERIAL_PORT;
    int baudrate = BAUDRATE;

    fd = serialport_init(serialport, baudrate);

    if (fd < 0){
        printError("serial port initialization failed");
        return fd;
    }

    printf("successfully opened serialport %s @ %d bps\n", serialport, baudrate);
    serialport_flush(fd);

    context->file_descriptor = fd;

    return 0;
}

int destroy_serial_connection(IfaceContext *context){
    int error_code = serialport_close(context->file_descriptor);
    return error_code;
}

int expect_confirmation(IfaceContext *context){
    int error_code = serialport_read_until(
            context->file_descriptor,
            context->scratch_buffer.data,
            '0',
            2,
            1000
            );

    if(error_code < 0){
        printError("serialport_read_until failed");
        return error_code;
    }

    char * expected_message = "ok";
    if(strncmp(context->scratch_buffer.data, expected_message, 2) != 0){
        printf("error on expected result from serial");
        return -1;
    }

    return error_code;
}

int serial_send_byte(IfaceContext *context, uint8_t byte_message){
    int error_code;
    int fd = context->file_descriptor;

    error_code = serialport_writebyte(fd, byte_message);
    if(error_code < 0){
        printError("writing to serial port failed");
        return error_code;
    }

    if(byte_message == 3){
        error_code = expect_confirmation(context);
        if(error_code < 0){
            printError("expect_confirmation failed");
            return error_code;
        }
    }

    return error_code;
}

int send_ping(IfaceContext *context){
    return serial_send_byte(context, 3);
}

int serial_send_struct(IfaceContext *context, void *message, size_t len){
    char *char_message = (char *) message;

    if (send_ping(context) < 0){
        printError("ping couldn't be completed");
        return -1;
    }

    for (int i=0; i<len; i++) {
        if (serial_send_byte(context, char_message[i]) < 0) {
            printError("error sending bytes");
            return -1;
        }
    }

    return 0;
}
