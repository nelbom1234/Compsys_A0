#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct index_record {
    int64_t osm_id;
    const struct record* record;
};

struct indexed_data {
    struct index_record *irs;
    int n;
};

struct indexed_data* mk_indexed(struct record* rs, int n) {
    struct index_record records[n];
    struct indexed_data* data = malloc(sizeof(records)*n+sizeof(n));
    for (int i = 0; i < n-1; i++) {
        records[i].osm_id = rs[i].osm_id;
        records[i].record = &rs[i];
    }
    data->irs = records;
    data->n = n;
    return data;
}

void free_indexed(struct indexed_data* data) {
    free(data);
}

const struct record* lookup_indexed(struct indexed_data *data, int64_t needle) {
    int len = data->n;
    for (int i = 0; i < len-1; i++) {
        if (data->irs[i].osm_id == needle) {
            return data->irs[i].record;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_indexed,
                    (free_index_fn)free_indexed,
                    (lookup_fn)lookup_indexed);
}