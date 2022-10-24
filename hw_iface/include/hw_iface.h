//
// Created by Kevin Zeledón on 23/10/22.
//

#ifndef TEST_HW_IFACE_H
#define TEST_HW_IFACE_H

#include "include/hw_structs.h"
#include "include/serial_comm.h"

typedef enum direction_t {
    RIGHT,
    LEFT
} direction_t;

typedef enum ship_type_t {
    NO_SHIP,
    NORMAL,
    PESQUERO,
    PATRULLA
} ship_type_t;

int add_ship(iface_context_t *context, direction_t direction);
int change_channel_direction(iface_context_t *context, direction_t direction);
int interchange_ships_position(iface_context_t *context, int from_pos, int to_pos);
int place_ship_in_position(iface_context_t *context, int position, ship_type_t shipType);
int remove_ship_from_position(iface_context_t *context, int position);

#endif //TEST_HW_IFACE_H
