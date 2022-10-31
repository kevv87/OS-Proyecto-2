//
// Created by Kevin Zeledón on 29/10/22.
//
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmocka/include/cmocka.h"

#include "hw_iface/include/serial_comm.h"
#include "hw_iface/include/hw_structs.h"
#include "hw_iface/include/hw_iface.h"

iface_context_t * context;

int setup(){
    context = malloc(sizeof(iface_context_t));
    context->scratch_buffer.data = malloc(1024);
    context->scratch_buffer.len = 1024;

    context->state.ships_at_left = 0;
    context->state.ships_at_right = 0;
    context->state.channel_direction = 0;

    if (initialize_serial_connection(context) < 0)
        printError("Error initializing serial");
    return 0;
}

int teardown(){
    destroy_serial_connection(context);
    free(context->scratch_buffer.data);
    free(context);
    return 0;
}

static void test_send_first_boat(){
    //place_ship_in_position(context, 0, NORMAL);
    char * message = "Hola!";
    send_ping(context);
    serialport_write(context->file_descriptor, message);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_send_first_boat),
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}
