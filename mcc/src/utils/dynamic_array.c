#include <stdlib.h>
#include "mcc/dynamic_array.h"

Dynamic_Array *mcc_create_dynamic_array(const int MAX_SIZE) {
    Dynamic_Array *dyn_arr = malloc(sizeof(*dyn_arr) + sizeof(void*) * MAX_SIZE);

    if (dyn_arr == NULL) {
        return NULL;
    }

    dyn_arr -> max_size = MAX_SIZE;
    dyn_arr -> index = 0;

    return dyn_arr;
}

int mcc_add_to_array(Dynamic_Array *dyn_arr, void *elem) {
    int max_size = dyn_arr -> max_size;
    int index = dyn_arr -> index;

    if (index < max_size) {
        dyn_arr -> arr[index] = elem;
        dyn_arr -> index += 1;

        return 1;
    } else {
        dyn_arr = realloc(dyn_arr, sizeof(*dyn_arr) + sizeof(void*) * max_size);

        if (dyn_arr == NULL) {
            return 0;
        }

        dyn_arr -> arr[index] = elem;
        dyn_arr -> index += 1;
        dyn_arr -> max_size += max_size;

        return 1;
    }
}

static void generic_delete(void* ptr) {
    free(ptr);
}

void mcc_delete_array(Dynamic_Array *dyn_arr, void (*delete)(void*)) {
    void (*delete_f) (void*) = &generic_delete;

    if (delete == NULL) {
        delete_f = delete;
    }

    for (int i = 0; i <= dyn_arr -> index; i++) {
        delete_f(dyn_arr -> arr[i]);
    }
}

