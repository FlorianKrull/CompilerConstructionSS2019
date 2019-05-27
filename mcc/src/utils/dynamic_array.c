#include <stdlib.h>
#include "mcc/dynamic_array.h"

Dynamic_Array *mcc_create_dynamic_array(int MAX_SIZE) {

    Dynamic_Array *dyn_arr = malloc(sizeof(*dyn_arr));

    if (dyn_arr == NULL) {
        return NULL;
    }

    dyn_arr -> init_max_size = MAX_SIZE;
    dyn_arr -> block_size = MAX_SIZE;
    dyn_arr -> size = 0;
    dyn_arr -> arr = malloc(sizeof(void*) * MAX_SIZE);

    return dyn_arr;
}

int mcc_add_to_array(Dynamic_Array *dyn_arr, void *elem ) {

    int init_max_size = dyn_arr -> init_max_size;
    int block_size = dyn_arr -> block_size;

    int size = dyn_arr -> size;

    if (size < block_size) {
        dyn_arr -> arr[size] = elem;
        dyn_arr -> size += 1;

        return 0;
    } else {
        void *array = dyn_arr -> arr;
        void *new_arr = realloc(array, sizeof(void*) * (block_size + init_max_size));
        if (new_arr == NULL) return 1;

        dyn_arr -> arr = new_arr;
        dyn_arr -> arr[size] = elem;
        dyn_arr -> size += 1;
        dyn_arr -> block_size += init_max_size;

        return 0;
    }
}

static void generic_delete(void* ptr) {
    free(ptr);
}

void mcc_delete_array(Dynamic_Array *dyn_arr, void (*delete_func)(void*)) {
    void (*delete_f) (void*) = generic_delete;

    if (delete_func == NULL) {
        delete_func = delete_f;
    }

    for (int i = 0; i < dyn_arr -> size; i++) {
        if (dyn_arr -> arr[i] != NULL) {
            delete_func(dyn_arr -> arr[i]);
        }
    }

    free(dyn_arr -> arr);
    free(dyn_arr);
}
