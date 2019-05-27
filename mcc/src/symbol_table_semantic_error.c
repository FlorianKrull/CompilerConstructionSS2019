#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mcc/symbol_table_semantic_error.h"


const int MCC_ERROR_COLLECTOR_ERROR_MAX = 10;
const int MCC_TYPE_TRACER_SIZE = 100;

// ---------------------------------------------------------- Error

struct mcc_symbol_table_error_collector *mcc_symbol_table_new_error_collector() {
    int error_size = sizeof(struct mcc_parser_error*) * MCC_ERROR_COLLECTOR_ERROR_MAX;

    struct mcc_symbol_table_error_collector *ec = malloc(sizeof(*ec) + error_size);

    ec -> type_tracer = mcc_create_dynamic_array(MCC_TYPE_TRACER_SIZE);
    ec -> error_max = MCC_ERROR_COLLECTOR_ERROR_MAX;
    ec -> error_size = 0;

    return ec;
}

void mcc_symbol_table_delete_error_collector(struct mcc_symbol_table_error_collector *ec) {
    assert(ec);

    for (int i = 0; i < ec -> error_size; i++) {
        // delete_parser_error(ec -> errors[i]);
    }

    free(ec);
}

struct mcc_semantic_error *mcc_symbol_table_new_error(struct mcc_ast_source_location *loc, enum mcc_semantic_error_type type) {
    assert(loc);

    struct mcc_semantic_error *error = malloc(sizeof(*error));

    error -> error_type = type;
    error -> sloc = loc;

    return error;
}

int mcc_symbol_table_add_error(struct mcc_symbol_table_error_collector *ec, struct mcc_semantic_error *er) {
    assert(ec);
    assert(er);

    int er_size = ec -> error_size;
    int er_max = ec -> error_max;

    if (er_size < er_max) {
        ec -> errors[er_size] = er;

        return 1;
    } else {
        int error_size = sizeof(struct mcc_semantic_error*) * MCC_ERROR_COLLECTOR_ERROR_MAX;
        ec = realloc(ec, sizeof(*ec) + error_size);

        ec -> errors[er_size] = er;
        ec -> error_max += MCC_ERROR_COLLECTOR_ERROR_MAX;

        return 1;
    }
}

void mcc_symbol_table_add_type_check(
        struct mcc_symbol_table_error_collector *ec,
        enum mcc_ast_data_type target_type,
        enum mcc_ast_data_type receiving_type,
        enum mcc_semantic_type_check_type type,
        struct mcc_ast_source_location *sloc) {

    struct type_check *tc = malloc(sizeof(*tc));

    tc -> sloc = sloc;
    tc -> receiving_type = receiving_type;
    tc -> target_type = target_type;
    tc -> type = type;

    mcc_add_to_array(ec -> type_tracer, tc);
}

void mcc_symbol_table_delete_type_check(struct mcc_symbol_table_error_collector *ec) {
    mcc_delete_array(ec -> type_tracer, NULL);
}
