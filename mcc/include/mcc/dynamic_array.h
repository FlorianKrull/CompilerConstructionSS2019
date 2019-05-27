//
// Created by Clemens Paumgarten on 23.05.19.
//

#ifndef MCC_DYNAMIC_ARRAY_H
#define MCC_DYNAMIC_ARRAY_H

typedef struct dyn_array {
    int index;
    int init_max_size;
    int size;
    void *arr[];
} Dynamic_Array;

Dynamic_Array *mcc_create_dynamic_array(int MAX_SIZE);

int mcc_add_to_array(Dynamic_Array *dyn_arr, void *elem);

void mcc_delete_array(Dynamic_Array *dyn_arr, void (*delete)(void*));

#endif //MCC_DYNAMIC_ARRAY_H
