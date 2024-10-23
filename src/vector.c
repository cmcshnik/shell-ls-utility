#include "vector.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// <-- YOUR CODE HERE -->

GenericVector* NewGenericVector(size_t capacity) {
    GenericVector* vector = malloc(sizeof(GenericVector));
    if (!vector) {
        return NULL;
    }

    vector->arr_ = malloc(sizeof(void*) * capacity);
    if (!vector->arr_) {
        free(vector);
        return NULL;
    }

    vector->capacity_ = capacity;
    vector->len_ = 0;
    return vector;
}

void FreeGenericVector(GenericVector* vector, void (*FreeElementFunc)(void*)) {
    for (int i = 0; i < vector->len_; ++i) {
        if (vector->arr_[i] != NULL) {
            (*FreeElementFunc)(vector->arr_[i]);
        }
    }

    free(vector->arr_);
    free(vector);
}

bool Append(GenericVector* vector, void* elem) {
    if (vector->len_ == vector->capacity_) {
        vector->capacity_ *= 2;
        vector->arr_ = realloc(vector->arr_, vector->capacity_ * sizeof(void*));
        if (!vector->arr_) {
            return false;
        }
    }

    vector->arr_[vector->len_] = elem;
    vector->len_++;
    return true;
}

void* GetElement(const GenericVector* vector, size_t idx) {
    if (idx < 0 || idx >= vector->len_) {
        perror("Wrong index!");
    }
    return vector->arr_[idx];
}

void** GetData(GenericVector* vector) {
    return vector->arr_;
}

size_t GetLength(const GenericVector* vector) {
    return vector->len_;
}

bool IsEmpty(const GenericVector* vector) {
    for (size_t i = 0; i < vector->len_; ++i) {
        if (vector->arr_[i] != NULL) {
            return false;
        }
    }
    return true;
}

void DeleteElement(GenericVector* vector, int idx, void (*FreeElementFunc)(void*)) {
    if (idx < 0 || idx >= vector->len_) {
        errno = EINVAL;
        perror("Wrong index");
    }

    (*FreeElementFunc)(vector->arr_[idx]);

    for (int i = idx; i < vector->len_ - 1; ++i) {
        vector->arr_[i] = vector->arr_[i + 1];
    }

    vector->len_ --;
}

void ReverseGenericVector(GenericVector* vector) {
    if (vector == NULL || vector->len_ == 0) {
        return;  
    }

    size_t left = 0;
    size_t right = vector->len_ - 1;
    void* temp;

    while (left < right) {
        temp = vector->arr_[left];
        vector->arr_[left] = vector->arr_[right];
        vector->arr_[right] = temp;

        left++;
        right--;
    }
}