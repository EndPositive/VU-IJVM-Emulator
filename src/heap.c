#include <heap.h>
#include <stdlib.h>

arrays_t *arrays;

int init_arrays() {
    arrays = (arrays_t *)malloc(sizeof(arrays_t));
    arrays->size = 0;
    return 1;
}

void destroy_arrays() {
    for (int i = 0; i < arrays->size; i++) {
        free(arrays->arrays[i]->data);
        free(arrays->arrays[i]);
    }
    if (arrays->size > 0) {
        free(arrays->arrays);
    }
    free(arrays);
}

word_t new_array(int size) {
    if (arrays->size == 0) {
        arrays->arrays = (array_t **)malloc(sizeof(array_t*));
    } else {
        arrays->arrays = (array_t **)realloc(arrays->arrays, (arrays->size + 1) * sizeof(array_t*));
    }

    array_t *array = (array_t *)malloc(size * sizeof(array_t));

    arrays->size++;
    arrays->arrays[arrays->size - 1] = array;

    array->size = size;
    array->data = (word_t *)malloc(size * sizeof(word_t));
    array->ref = arrays->size - 1;
    return array->ref;
}

array_t *get_array(word_t ref) {
    for (word_t i = 0; i < arrays->size; i++) {
        if (arrays->arrays[i]->ref == ref) {
            return arrays->arrays[i];;
        }
    }
    return NULL;
}
