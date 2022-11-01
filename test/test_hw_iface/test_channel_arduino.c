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

int turn_off_leds(){
    remove_ship_from_position(context, 0);
    remove_ship_from_position(context, 1);
    remove_ship_from_position(context, 2);
    remove_ship_from_position(context, 3);
    remove_ship_from_position(context, 4);
}

static void test_place_first_boat(){
    place_ship_in_position(context, 0, NORMAL);
}

static void test_remove_first_boat(){
    remove_ship_from_position(context, 0);
}

static void test_add_diferent_boats_same_spot(){
    place_ship_in_position(context, 0, NORMAL);
    place_ship_in_position(context, 0, PESQUERO);
    place_ship_in_position(context, 0, PATRULLA);
    place_ship_in_position(context, 0, NO_SHIP);
}

static void test_add_boats_different_spots(){
    place_ship_in_position(context, 0, NORMAL);
    place_ship_in_position(context, 1, NORMAL);
    place_ship_in_position(context, 2, NORMAL);
    place_ship_in_position(context, 3, NORMAL);
    place_ship_in_position(context, 4, NORMAL);

    turn_off_leds();
}

static void test_move_ships(){
    place_ship_in_position(context, 0, NORMAL);
    move_ships_position(context, 0, 3);
    //turn_off_leds();
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_place_first_boat),
            cmocka_unit_test(test_remove_first_boat),
            cmocka_unit_test(test_add_diferent_boats_same_spot),
            cmocka_unit_test(test_add_boats_different_spots),
            cmocka_unit_test(test_move_ships),
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}
