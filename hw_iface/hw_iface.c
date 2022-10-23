//
// Created by Kevin Zeledón on 15/10/22.
//

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
            1,
            1000
            );

    if(error_code < 0){
        printError("serialport_read_until failed");
        return error_code;
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

    if(byte_message == 0){
        error_code = expect_confirmation(context);
        if(error_code < 0){
            printError("expect_confirmation failed");
            return error_code;
        }
    }

    return error_code;
}
