//
// Created by Clemens Paumgarten on 09.05.19.
//

#ifndef MCC_MCC_SCOPE_H
#define MCC_MCC_SCOPE_H

#include "mcc/ast.h"

struct mcc_ast_function *mcc_limit_scope_to_function(char* function_name, struct mcc_ast_program* p);

#endif //MCC_MCC_SCOPE_H
