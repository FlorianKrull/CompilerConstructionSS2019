#include <stdlib.h>
#include <stdio.h>
#include "mcc/dynamic_array.h"

Dynamic_Array *mcc_create_dynamic_array(const int MAX_SIZE) {
    Dynamic_Array *dyn_arr = malloc(sizeof(*dyn_arr) + sizeof(void*) * MAX_SIZE);

    if (dyn_arr == NULL) {
        return NULL;
    }

    dyn_arr -> init_max_size = MAX_SIZE;
    dyn_arr -> size = MAX_SIZE;
    dyn_arr -> index = 0;

    return dyn_arr;
}

int mcc_add_to_array(Dynamic_Array *dyn_arr, void *elem ) {
    int init_max_size = dyn_arr -> init_max_size;
    int size = dyn_arr -> size;
    int index = dyn_arr -> index;

    if (index < size) {
        dyn_arr -> arr[index] = elem;
        dyn_arr -> index += 1;

        return 0;
    } else {

        Dynamic_Array *rea_arr = realloc(dyn_arr, sizeof(*dyn_arr) + sizeof(void*) * init_max_size);

        if (rea_arr == NULL) {
            perror("Realloc failed");
            // clean on error
            return 1;
        }

        dyn_arr = rea_arr;

        dyn_arr -> arr[index] = elem;
        dyn_arr -> index += 1;
        dyn_arr -> size += init_max_size;

        return 0;
    }
}

static void generic_delete(void* ptr) {
    free(ptr);
}

void mcc_delete_array(Dynamic_Array *dyn_arr, void (*delete_func)(void*)) {
    void (*delete_f) (void*) = &generic_delete;

    if (delete_func == NULL) {
        delete_func = delete_f;
    }

    for (int i = 0; i <= dyn_arr -> index; i++) {
        delete_func(dyn_arr -> arr[i]);
    }
}

