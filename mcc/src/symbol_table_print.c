#include <stdio.h>
#include <stdlib.h>

#include "mcc/symbol_table_print.h"

void mcc_symbol_table_print_error(struct mcc_symbol_table_error_collector *ec, FILE *out) {
    // for now only one error in collector
    struct mcc_semantic_error *error = ec -> errors[0];

    if (error != NULL) {
        fprintf(out, "Semantic error in line: %d \n", error -> sloc ->end_line);
        fprintf(out, "Semantic error type: %d \n", error->error_type);
    }

};