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

struct node* kdtree(struct record* points, int depth) {
    int axis = depth % 2;
    int len = sizeof(points)/sizeof(points[0]);
    if (!axis)
        qsort(points, len, sizeof(struct record), compareLon);
    else 
        qsort(points,len,sizeof(struct record),compareLat);
    
} 

void free_kdtree(struct node* node) {
    struct node* node_left = node->left;
    struct node* node_right = node->right;
    free(node);
    if (node_left != NULL) {
        free_kdtree(node_left);
    }
    if (node_right != NULL) {
        free_kdtree(node_right);
    }
}
