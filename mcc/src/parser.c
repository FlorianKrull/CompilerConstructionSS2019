//
// Created by Clemens Paumgarten on 19.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mcc/parser.h"

struct mcc_parser_error *new_parser_error(struct mcc_ast_source_location *loc, char *msg) {
    assert(loc);
    assert(msg);

    char *error_msg = (char*) malloc(1024 * sizeof(char));

    sprintf(error_msg, "Parser Error: \n "
            "%s \n"
            "Location: Line: %d, Column, %d \n", msg, loc->start_line, loc->end_col);

    struct mcc_parser_error *error = malloc(sizeof(*error));

    error -> loc = loc;
    error -> error_msg = error_msg;

    return error;
}