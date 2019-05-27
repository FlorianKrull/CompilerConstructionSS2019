#ifndef MCC_DYNAMIC_ARRAY_H
#define MCC_DYNAMIC_ARRAY_H

typedef struct dyn_array {
    int init_max_size;
    int block_size;
    int size;
    void** arr;
} Dynamic_Array;

Dynamic_Array *mcc_create_dynamic_array(int MAX_SIZE);

int mcc_add_to_array(Dynamic_Array *dyn_arr, void *elem);

void mcc_delete_array(Dynamic_Array *dyn_arr, void (*delete_f)(void*));

#endif //MCC_DYNAMIC_ARRAY_H
