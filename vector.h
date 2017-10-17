#ifndef VECTOR_H
#define VECTOR_H

typedef struct vector {
    int size;
    int cap;
    char** data;
} vector;

vector* make_vector();
void  free_vector(vector* sv);

char* vector_get(vector* sv, int ii);
void  vector_put(vector* sv, int ii, char* item);

void vector_push_back(vector* sv, char* item);

int vector_contains(vector* sv, char* item, size_t charsToCompare);

#endif
