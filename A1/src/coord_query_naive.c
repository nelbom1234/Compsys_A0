#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

struct naive_data {
  struct record *rs;
  int n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  struct naive_data* data = malloc(sizeof(rs)+sizeof(n));
  data->rs = rs;
  data->n = n;
  return data;
  
}

void free_naive(struct naive_data* data) {
  free(data);
}

const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
  int len = data->n;
  int index = 0;
  double indexDist;
  //intitialize distance for first value to have something to compare with
  indexDist = sqrt(pow(data->rs[0].lon-lon, 2.0)+pow(data->rs[0].lat-lat,2.0));
  for (int i = 1; i < len-1; i++) {
    double dist;
    dist = sqrt(pow(data->rs[i].lon-lon,2.0) + pow(data->rs[i].lat-lat,2.0));
    if (dist < indexDist) {
      indexDist = dist;
      index = i;
    }
  }
  return &data->rs[index];
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
