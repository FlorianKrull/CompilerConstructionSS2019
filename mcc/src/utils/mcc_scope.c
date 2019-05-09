#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mcc/mcc_scope.h"

struct mcc_ast_function *mcc_limit_scope_to_function(char* function_name, struct mcc_ast_program* p) {
    assert(function_name);
    assert(p);

    struct mcc_ast_function *f;
    for (int i = 0; i < p -> size; i++) {
        f = p -> function_def[i];

        if (f != NULL) {
            if (strcmp(function_name, f -> identifier ->i_value) == 0) {
                return f;
            }
        }
    }

    return NULL;
}
