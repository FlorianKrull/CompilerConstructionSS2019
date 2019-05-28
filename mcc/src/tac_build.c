#include "mcc/tac_build.h"


struct mcc_tac *mcc_new_tac(enum mcc_tac_operation op,
                            struct mcc_tac *prev,
                            struct mcc_tac *next)
{
    struct mcc_tac *tac = malloc(sizeof(tac));

    tac -> tac_op = op;
    tac -> arg1 = NULL;
    tac -> arg2 = NULL;
    tac -> result = NULL;

    tac -> prev = prev;
    tac -> next = next;

    return tac;

}