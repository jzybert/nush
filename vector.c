#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "vector.h"

vector*
make_vector()
{
    vector* v = malloc(sizeof(vector));
    v->size = 0;
    v->cap  = 2;
    v->data = malloc(2 * sizeof(char*));
    memset(v->data, 0, 2 * sizeof(char*));
    return v;
}

void
free_vector(vector* v)
{
    int ii;
    for (ii = 0; ii < v->size; ++ii) {
	free(v->data[ii]);
    }
    free(v->data);
    free(v);
}

char*
vector_get(vector* v, int ii)
{
    assert(ii >= 0 && ii < v->size);
    return v->data[ii];
}

void
vector_put(vector* v, int ii, char* item)
{
    assert(ii >= 0 && ii < v->size);
    v->data[ii] = strdup(item);

}

void 
vector_push_back(vector* v, char* item)
{
    int ii = v->size;

    if (ii >= v->cap) {
        v->cap *= 2;
        v->data = (char**) realloc(v->data, v->cap * sizeof(char*));
    }
    
    v->size = ii + 1;
    vector_put(v, ii, item);
}

int
vector_contains(vector* sv, char* item, size_t charsToCompare)
{
	int ii;
	for (ii = 0; ii < sv->size; ++ii) {
		if (strncmp(vector_get(sv, ii), item, charsToCompare) == 0) {
			return 1;
		}
	}
	return 0;
}










