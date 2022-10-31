//
// Created by Kevin Zeledón on 19/10/22.
//

#ifndef HW_STRUCTS_H
#define HW_STRUCTS_H

#include <stdint.h>

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

typedef struct iface_state_t{
    ship_type_t channel_representation[10];
    uint8_t ships_at_left;
    uint8_t ships_at_right;
    direction_t channel_direction;
} iface_state_t;

typedef struct buffer_t{
    char* data;
    int len;
} buffer_t;

typedef struct iface_context_t{
    int file_descriptor;
    buffer_t scratch_buffer;
    iface_state_t state;
} iface_context_t;


#endif //HW_STRUCTS_H
