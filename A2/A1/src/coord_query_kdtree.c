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

int compareLon(const void * a, const void * b) {
    const struct record *A = (struct record*) a;
    const struct record *B = (struct record*) b;
    return (int)(A->lon - B->lon); 
}

int compareLat(const void * a, const void * b) {
    const struct record *A = (struct record*) a;
    const struct record *B = (struct record*) b;
    return (int)(A->lat - B->lat); 
}

struct node {
    int axis;
    struct node *left;
    struct node *right;
    struct record* point;
};


struct node* kdtree(struct record* points, int depth, int n) {
    struct node* node;
    if (n != 0) {
        int axis = depth % 2;
        if (!axis)
            qsort(points, n, sizeof(struct record), compareLon);
        else 
            qsort(points, n, sizeof(struct record), compareLat);
        int medianIndex = floor((n-1)/2);
        struct record median = points[medianIndex];
        node = malloc(sizeof(struct node));
        node->point = &median;
        node->axis = axis;
        node->left = kdtree(points,depth+1, medianIndex);
        node->right = kdtree(points+(medianIndex+1),depth+1,medianIndex + 1 - (n % 2));
    }
    return node;
} 

void free_kdtree(struct node* node) {
    struct node* node_left = node->left;
    struct node* node_right = node->right;
    if (node_left != NULL) {
        free_kdtree(node_left);
    }
    if (node_right != NULL) {
        free_kdtree(node_right);
    }
    free(node);
}


void search_kdtree(struct record* closest, double *query, struct node *node) {
    if (node == NULL) {
        return;
    }
    double closestDist = sqrt(pow(closest->lon-query[0],2.0)+pow(closest->lat-query[1],2.0));
    double nodeDist = sqrt(pow(node->point->lon-query[0],2.0)+pow(node->point->lat-query[1],2.0));
    
    if (nodeDist < closestDist) {
        closest = node->point;
    }
    double diff;
    if (node->axis == 0) {
        diff = node->point->lon-query[0];
    }
    else
        diff = node->point->lat-query[1];
    
    //radius from the pseudo-code has already been calculated as nodeDist
    if (diff >= 0 || nodeDist > fabs(diff)) {
        search_kdtree(closest, query, node->left);
    }
    if (diff <= 0 || nodeDist > fabs(diff)) {
        search_kdtree(closest, query, node->right);
    }
}

//so we have a function that actually works with coord_query.c usage
struct node* mk_kdtree(struct record* points, int n) {
    return kdtree(points, 0, n);
}

//same comment as mk_kdtree
const struct record* lookup_kdtree(struct node *node, double lon, double lat) {
    double query[2];
    query[0] = lon;
    query[1] = lat;
    struct record closest = *(node->point);
    search_kdtree(&closest, query, node);
    return &closest;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}