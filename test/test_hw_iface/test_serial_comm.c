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

#include "hw_iface/include/hw_iface.h"
#include "hw_iface/include/hw_structs.h"

iface_context * context;

int setup(){
    context = malloc(sizeof(iface_context));
    initialize_serial_connection(context);
    return 0;
}

int teardown(){
    destroy_serial_connection(context);
    free(context);
    return 0;
}

static void test_serial_init(){
    int error_code;
    context = malloc(sizeof(iface_context));

    error_code = initialize_serial_connection(context);
    assert_int_equal(error_code, 0);
    assert_true(context->file_descriptor >= 0);
}

static void test_send_byte(){
    uint8_t message_byte = 80;
    int error_code;

    error_code = serial_send_byte(context, message_byte);

    assert_true(error_code>=0);
}

static void test_send_serial_struct(){
    assert_true(true);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test_teardown(test_serial_init, teardown),
            cmocka_unit_test_setup_teardown(test_send_byte, setup, teardown),
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
    return cmocka_run_group_tests(tests, NULL, NULL);
}