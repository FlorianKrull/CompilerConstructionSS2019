#include <CuTest.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcc/ast.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/symbol_table_parse.h"
#include "mcc/symbol_table_validate.h"
#include "mcc/symbol_table_semantic_error.h"

struct mcc_ast_program *get_result_from_program(char *file_name) {
    FILE *in;
    in = fopen(file_name, "r");

    if (in == NULL) {
        perror("File is NULL");
    }

    struct mcc_ast_program *prog = NULL;

    {
        struct mcc_parser_result result = mcc_parse_file(in);
        if (result.status != MCC_PARSER_STATUS_OK) {
            // print error message
            printf("%s", result.parser_error -> error_msg);

            return NULL;
        }
        prog = result.program;
    }

    fclose(in);

    return prog;
}

void clean_pointers(
        struct mcc_ast_program *p,
        struct mcc_symbol_table *st,
        struct mcc_symbol_table_error_collector *ec) {
    free(ec);
    free(st);
    free(p);
}

void BinaryOPDifferentTypes(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/binary_op_different_types.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_SAME_TYPE, ec -> errors[0] -> error_type);

        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void FuncAlreadyDeclared(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/func_already_declared.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_FUNC_ALREADY_DECLARED, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void ArrayAlreadyDeclared(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/array_already_declared.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_ARRAY_ALREADY_DECLARED, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void ArraySizeDefinitionInt(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/array_size_definition.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_ARRAY_SIZE_DEFINITION, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void VariableNotDeclared(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/variable_not_declared.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_VARIABLE_NOT_DECLARED, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void VariableAlreadyDeclared(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/variable_already_declared.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_VARIABLE_ALREADY_DECLARED, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void FuncNotDeclared(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/func_not_declared.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_FUNC_NOT_DECLARED, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void WrongArgumentType(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/wrong_argument_type.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_WRONG_ARGUMENT_TYPE, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void WrongNumOfArguments(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/wrong_num_of_arguments.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_WRONG_NUM_OF_ARGUMENTS, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void AssignmentWrongType(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/type_assignment.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_TYPE_ASSIGNMENT, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void MainMissing(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/main_missing.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_MAIN_MISSING, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void UnaryOpExpectedBool(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/unary_op_expected_bool.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_BOOL, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void UnaryOpExpectedNumber(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/unary_op_expected_number.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_UNARY_OP_EXPECTED_NUMBER, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void BinaryOpHandsideSameType(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/binary_op_handside_same_type.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_SAME_TYPE, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void BinaryOpHandsideBoolType(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/binary_op_handside_bool_type.mc");

    if (prog != NULL) {
        struct mcc_symbol_table_error_collector *ec =  mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_BOOL_TYPE, ec -> errors[0] -> error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void BinaryOpHandsideNumberType(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/binary_op_handside_number_type.mc");

    if(prog != NULL) {
        struct mcc_symbol_table_error_collector *ec = mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_BINARY_OP_HANDSIDE_NUMBER_TYPE, ec->errors[0]->error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void BinaryOpHandsideDivisionByZero(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/binary_op_div_by_0.mc");

    if(prog != NULL) {
        struct mcc_symbol_table_error_collector *ec = mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_BINARY_OP_DIV_BY_0, ec->errors[0]->error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}

void ConditionBoolExpected(CuTest *ct) {
    struct mcc_ast_program *prog = get_result_from_program("../test/semantic/condition_bool_expected.mc");

    if(prog != NULL) {
        struct mcc_symbol_table_error_collector *ec = mcc_symbol_table_new_error_collector();
        struct mcc_symbol_table *symbol_table = mcc_symbol_table_build(prog, ec);

        CuAssertIntEquals(ct, MCC_SEMANTIC_ERROR_CONDITION_BOOL_EXPECTED, ec->errors[0]->error_type);
        clean_pointers(prog, symbol_table, ec);
    } else {
        perror("AST is NULL");
    }
}


#define TESTS \
        TEST(BinaryOPDifferentTypes) \
        TEST(ArrayAlreadyDeclared) \
        TEST(ArraySizeDefinitionInt) \
        TEST(FuncAlreadyDeclared) \
        TEST(VariableAlreadyDeclared) \
        TEST(VariableNotDeclared) \
        TEST(FuncNotDeclared) \
        TEST(WrongArgumentType) \
        TEST(WrongNumOfArguments) \
        TEST(AssignmentWrongType) \
        TEST(MainMissing) \
        TEST(UnaryOpExpectedBool) \
        TEST(UnaryOpExpectedNumber) \
        TEST(BinaryOpHandsideSameType) \
        TEST(BinaryOpHandsideBoolType) \
        TEST(BinaryOpHandsideNumberType) \
        TEST(BinaryOpHandsideDivisionByZero) \
        TEST(ConditionBoolExpected) \

#include "main_stub.inc"
#undef TESTS
