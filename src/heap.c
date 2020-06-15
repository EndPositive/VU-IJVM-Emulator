#include <heap.h>
#include <frame.h>
#include <stdlib.h>
#include <time.h>

arrays_t *arrays;

int init_arrays() {
    srand(time(NULL));
    arrays = (arrays_t *)malloc(sizeof(arrays_t));
    arrays->size = 0;
    arrays->arrays = (array_t **)malloc(sizeof(array_t*));
    return 1;
}

void destroy_arrays() {
    for (int i = 0; i < arrays->size; i++) {
        free(arrays->arrays[i]->data);
        free(arrays->arrays[i]);
    }

    free(arrays->arrays);
    free(arrays);
}

word_t new_array(int size) {
    arrays->size++;
    arrays->arrays = (array_t **)realloc(arrays->arrays, (arrays->size + 1) * sizeof(array_t*));

    array_t *array = (array_t *)malloc(size * sizeof(array_t));
    arrays->arrays[arrays->size - 1] = array;

    array->size = size;
    array->data = (word_t *)malloc(size * sizeof(word_t));
    array->ref = rand();
    return array->ref;
}

array_t *get_array(word_t ref) {
    for (word_t i = 0; i < arrays->size; i++) {
        if (arrays->arrays[i]->ref == ref) return arrays->arrays[i];
    }
    return NULL;
}

void detect_garbage() {
    arrays_t *new_arrays = (arrays_t *)malloc(sizeof(arrays_t));
    new_arrays->size = 0;

    for (int i = 0; i < arrays->size; i++) {
        if (find_array_in_frame(frame, arrays->arrays[i])) {
            new_arrays->size++;
            arrays->arrays[i]->wipe = false;
        } else arrays->arrays[i]->wipe = true;
    }

    if (new_arrays->size > 0) {
        new_arrays->arrays = (array_t **)malloc(new_arrays->size * sizeof(array_t*));
        new_arrays->size = 0;
        for (int i = 0; i < arrays->size; i++) {
            if (!arrays->arrays[i]->wipe) {
                new_arrays->size++;
                new_arrays->arrays[new_arrays->size - 1] = arrays->arrays[i];
            } else {
                free(arrays->arrays[i]->data);
                free(arrays->arrays[i]);
            }
        }

        free(arrays->arrays);
        free(arrays);
        arrays = new_arrays;
    } else free(new_arrays);
}

bool find_array_in_frame(frame_t *current_frame, array_t *array_to_find) {
    bool available =
            find_array_in_frame_data(current_frame->local_size, current_frame->local_data, array_to_find) ||
            find_array_in_frame_data(current_frame->stack_size, current_frame->stack_data, array_to_find);

    if (available) return true;
    else if (current_frame->prev_frame != NULL) return find_array_in_frame(current_frame->prev_frame, array_to_find);
    else return false;
}

bool find_array_in_frame_data(int size, const word_t *data, array_t *array_to_find) {
    for (int i = 0; i < size; i++) {
        word_t ref = (word_t) data[i];
        array_t *array = get_array(ref);
        if (array != NULL && array == array_to_find) return true;
    }
    return false;
}
