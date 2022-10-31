//
// Created by Kevin Zeledón on 15/10/22.
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

iface_context_t * context;

int setup(){
    context = malloc(sizeof(iface_context_t));
    context->scratch_buffer.data = malloc(1024);
    context->scratch_buffer.len = 1024;
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

static void test_serial_init(){
    int error_code;
    iface_context_t *context;
    context = malloc(sizeof(iface_context_t));
    context->scratch_buffer.len = 1024;
    context->scratch_buffer.data = malloc(sizeof(char) * context->scratch_buffer.len);

    error_code = initialize_serial_connection(context);
    assert_int_equal(error_code, 0);
    assert_true(context->file_descriptor >= 0);
    destroy_serial_connection(context);
}

static void test_send_byte(){
    uint8_t message_byte = 20;
    int error_code;

    error_code = serial_send_byte(context, message_byte);

    assert_true(error_code>=0);
}

static void test_send_byte_with_confirmation(){
    // Sending byte 0 should trigger a confirmation message from arduino
    uint8_t message_byte = 0;
    int error_code;

    error_code = serial_send_byte(context, message_byte);

    assert_true(error_code==0);

    char *expected_returned_message = "ok";
    char *actual_message = context->scratch_buffer.data;
    assert_string_equal(expected_returned_message, actual_message);
}

static void test_send_struct(){
    iface_state_t hw_state = {
            .ledState = 200
    };

    int error_code;
    error_code = serial_send_struct(context, &hw_state, sizeof(iface_state_t));

    assert_true(error_code >= 0);

}

int main(void) {
    const struct CMUnitTest tests[] = {
            //cmocka_unit_test(test_serial_init),
            cmocka_unit_test(test_send_byte),
            //cmocka_unit_test_setup_teardown(test_send_byte_with_confirmation, setup, teardown),
            cmocka_unit_test(test_send_struct),
/*
            cmocka_unit_test(test_send_serial_struct),
            cmocka_unit_test_setup_teardown(test_add_first_item, setup_empty_list, teardown),
            cmocka_unit_test_setup_teardown(test_add_item_at_end, setup_empty_list, teardown),
            cmocka_unit_test_setup_teardown(test_add_several_items, setup_empty_list, teardown),
            cmocka_unit_test(test_generate_fixed_length_chunk),
            cmocka_unit_test_setup_teardown(test_get_pixel_by_index, setup_populated_list, teardown),
            cmocka_unit_test_setup_teardown(test_replace_first_pixel, setup_populated_list, teardown),
            cmocka_unit_test_setup_teardown(test_replace_nth_pixel, setup_populated_list, teardown),
            cmocka_unit_test_setup_teardown(test_get_pixel_by_metadata_id, setup_populated_list, teardown),
*/
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}