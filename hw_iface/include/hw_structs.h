//
// Created by Kevin Zeledón on 19/10/22.
//

#ifndef HW_STRUCTS_H
#define HW_STRUCTS_H

typedef struct hw_state_t{
    uint8_t ledState;
} hw_state_t;

typedef struct buffer_t{
    char* data;
    size_t len;
} buffer_t;

typedef struct IfaceContext{
    int file_descriptor;
    buffer_t scratch_buffer;
} IfaceContext;


#endif //HW_STRUCTS_H
