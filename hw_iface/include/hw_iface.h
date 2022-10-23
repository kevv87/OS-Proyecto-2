//
// Created by Kevin Zeledón on 15/10/22.
//

#ifndef HW_IFACE_H
#define HW_IFACE_H

#include "arduino-serial/arduino-serial-lib.h"
#include "common/include/tools.h"

#include "hw_structs.h"

#define SERIAL_PORT "/dev/cu.usbmodem1433101"
#define BAUDRATE 9600;

int initialize_serial_connection(IfaceContext *context);
int destroy_serial_connection(IfaceContext *context);

int serial_send_struct(IfaceContext *context, void *message, size_t len);
int serial_send_byte(IfaceContext *context, uint8_t byte_message);
int expect_confirmation(IfaceContext *context);


#endif //HW_IFACE_H
