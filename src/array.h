#ifndef ARRAY_H
#define ARRAY_H

int find_index(int *array, int len, int value) {
    for (int i = 0; i < len; i++)
        if (array[i] == value)
            return i;
    return -1;
}

void remove_index(int *array, int *len, int index) {
    for (int i = index; i < *len - 1; i++)
        array[i] = array[i + 1];
    (*len)--;
}

#endif  // ARRAY_H
