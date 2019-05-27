//
// Created by Clemens Paumgarten on 25.04.19.
//

#include <stdlib.h>
#include <assert.h>
#include "mcc/symbol_table_semantic_error.h"


const int MCC_ERROR_COLLECTOR_ERROR_MAX = 10;

// ---------------------------------------------------------- Error

struct mcc_symbol_table_error_collector *mcc_symbol_table_new_error_collector() {
    int error_size = sizeof(struct mcc_parser_error*) * MCC_ERROR_COLLECTOR_ERROR_MAX;

    struct mcc_symbol_table_error_collector *ec = malloc(sizeof(*ec) + error_size);

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
