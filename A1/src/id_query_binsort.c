#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "id_query.h"

//function for comparing 2 values for sorting
//returns a positive value if a is greater,
//a negative value if b is greater,
//or 0 if they are equal
//getting the types to not give errors when compiling courtesy of tutorialspoint.com: 
//https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
int compare(const void * a, const void * b) {
    const struct record *A = (struct record*) a;
    const struct record *B = (struct record*) b;
    return (int)(A->osm_id - B->osm_id);
}

//function for binary searching through the array
//returns the index if found or -1 if not
int binsearch(struct record* records, int64_t id, int low, int high) {
    if (high < low) {
        return -1;
    }
    int mid = floor((low+high)/2); 

    if (id == records[mid].osm_id) {
        return mid;
    }

    if (id > records[mid].osm_id) {
        return binsearch(records, id, mid+1, high);
    }
    else return binsearch(records, id, low, mid-1);
}

struct binsort_data {
    struct record *rs;
    int n;
};

struct binsort_data* mk_binsort(struct record* rs, int n) {
    struct binsort_data* data = malloc(sizeof(rs)+sizeof(n));
    data->rs = rs;
    qsort(data->rs, n, sizeof(struct record), compare);
    data->n = n;
    return data;
}

void free_binsort(struct binsort_data* data) {
    free(data);
}

const struct record* lookup_binsort(struct binsort_data *data, int64_t needle) {
    int index = binsearch(data->rs, needle, 0, data->n-1);
    if (index == -1) {
        return NULL;
    }
    return &data->rs[index];
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_binsort,
                    (free_index_fn)free_binsort,
                    (lookup_fn)lookup_binsort);
}