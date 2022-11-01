//
// Created by Kevin Zeledón on 23/10/22.
//

#include "hw_iface/include/hw_iface.h"

int add_ship(iface_context_t *context, direction_t direction) {
    if(direction == RIGHT){
        context->state.ships_at_left ++;
    }else if(direction == LEFT){
        context->state.ships_at_right ++;
    }

    // Send context to serial
    return serial_send_struct(context, context, sizeof(iface_context_t));
}

int change_channel_direction(iface_context_t *context, direction_t direction) {
    context->state.channel_direction = direction;
    // Send context to serial
    return serial_send_struct(context, context, sizeof(iface_context_t));
}

int move_ships_position(iface_context_t *context, int from_pos, int to_pos) {
    if (from_pos < 0 || from_pos > 10 || to_pos < 0 || to_pos > 10) {
        printf("Channel out of bounds!\n");
        return -1;
    }

    context->state.channel_representation[to_pos] = context->state.channel_representation[from_pos];
    context->state.channel_representation[from_pos] = NO_SHIP;
    // send context
    serial_send_byte(context, 2);
    serial_send_byte(context, from_pos);
    serial_send_byte(context, to_pos);
    serialport_flush(context->file_descriptor);
    return 0;
}

int place_ship_in_position(iface_context_t *context, int position, ship_type_t shipType) {
    if (position < 0 || position > 10) {
        printf("position out of bounds!\n");
    }
    context->state.channel_representation[position] = shipType;
    // send context
    serial_send_byte(context, 1);
    serial_send_byte(context, shipType);
    serial_send_byte(context, position);
    serialport_flush(context->file_descriptor);
    return 0;
}

int remove_ship_from_position(iface_context_t *context, int position) {
    context->state.channel_representation[position] = NO_SHIP;
    // send context
    serial_send_byte(context, 1);
    serial_send_byte(context, NO_SHIP);
    serial_send_byte(context, position);
    serialport_flush(context->file_descriptor);
    return 0;
}