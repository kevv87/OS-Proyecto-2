//
// Created by Kevin Zeledón on 19/10/22.
//

#ifndef HW_STRUCTS_H
#define HW_STRUCTS_H

#include "hw_iface.h"

typedef struct iface_state_t{
    int ships_at_left;
    int ships_at_right;
    direction_t channel_direction;
    ship_type_t channel_representation[10];
} iface_state_t;

typedef struct buffer_t{
    char* data;
    size_t len;
} buffer_t;

typedef struct iface_context_t{
    int file_descriptor;
    buffer_t scratch_buffer;
    iface_state_t state;
} iface_context_t;


#endif //HW_STRUCTS_H
