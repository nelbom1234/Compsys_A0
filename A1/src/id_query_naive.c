#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct naive_data {
  struct record *rs;
  int n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  struct naive_data* data = malloc(sizeof(rs)+sizeof(n));
  for (int i = 0; i < n; i++) {
    data->rs[n] = rs[n];
    data->n = n;
  }
  return data;

}

void free_naive(struct naive_data* data) {
  free(data);
}

const struct record* lookup_naive(struct naive_data *data, int64_t needle) {
  int len = data->n;
  for (int i = 0; i < len-1; i++) {
    if (data->rs[i]->osm_id == needle) {
      return data->rs;
    }
  }
  return NULL;
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_naive,
                    (free_index_fn)free_naive,
                    (lookup_fn)lookup_naive);
}
