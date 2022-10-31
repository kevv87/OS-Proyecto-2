//
// Created by Kevin Zeledón on 15/10/22.
//

#include <string.h>
#include <stdlib.h>
#include "include/serial_comm.h"

int initialize_serial_connection(iface_context_t *context){
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

int destroy_serial_connection(iface_context_t *context){
    int error_code = serialport_close(context->file_descriptor);
    return error_code;
}

int expect_confirmation(iface_context_t *context){
    int error_code = serialport_read_until(
            context->file_descriptor,
            context->scratch_buffer.data,
            '0',
            2,
            10000
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

int serial_send_byte(iface_context_t *context, uint8_t byte_message){
    int error_code;
    int fd = context->file_descriptor;

    error_code = serialport_writebyte(fd, byte_message);
    if(error_code < 0){
        printError("writing to serial port failed");
        return error_code;
    }

    if(byte_message == 170){
        error_code = expect_confirmation(context);
        if(error_code < 0){
            printError("expect_confirmation failed");
            return error_code;
        }
    }

    return error_code;
}

int send_ping(iface_context_t *context){
    int err_code = serial_send_byte(context, 170);
    printf("Ping completed!\n");
    return err_code;
}

int state_to_string(iface_state_t state, char *output, size_t *out_len){
    int j  = 0;
    for (int i = 0; i < 10; i++){
        output[j] = state.channel_representation[i];
        j += sizeof(ship_type_t);
        output[j] = ' ';
        j++;
    }
    output[j] = '\n';
    j++;

    output[j] = state.ships_at_left;
    j += sizeof(uint8_t);
    output[j] = ' ';
    j++;
    output[j] = state.ships_at_right;
    j += sizeof(uint8_t);
    output[j] = '\n';
    j++;

    output[j] = state.channel_direction;
    j += sizeof(uint8_t);

    output[j] = '\0';

    *out_len = j;
    return 0;
}

int serial_send_struct(iface_context_t *context, void *message, size_t len){
    // Now we are only sending the state struct
    char *char_message = malloc(1024);

    if (send_ping(context) < 0){
        printError("ping couldn't be completed");
        return -1;
    }

    state_to_string(context->state, char_message, &len);

    printf("Sending %zu bytes to arduino\n", len);
    printf("%s", char_message);
    printBuffer(char_message, len);
    serialport_write(context->file_descriptor, char_message);
//    for (int i=0; i<len; i++) {
//        if (serial_send_byte(context, char_message[i]) < 0) {
//            printError("error sending bytes");
//            return -1;
//        }
//    }

    return 0;
}
