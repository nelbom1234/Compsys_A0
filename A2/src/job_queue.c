#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "job_queue.h"

int job_queue_init(struct job_queue *job_queue, int capacity) {
  pthread_cond_init(&job_queue->cond_destroy, NULL);
  pthread_cond_init(&job_queue->cond_push, NULL);
  pthread_cond_init(&job_queue->cond_pop, NULL);

  //mutex to lock other threads running the function from modifying
  //important values mid calculation
  // pthread_mutex_init(&job_queue->mutex_destroy, NULL);
  // pthread_mutex_init(&job_queue->mutex_push, NULL);
  // pthread_mutex_init(&job_queue->mutex_pop, NULL);
  //mutex to lock other threads from modifying important values 
  //that we need to check if we should sleep or not
  pthread_mutex_init(&job_queue->mutex_general, NULL);
  job_queue->capacity = capacity;
  job_queue->size = 0;
  job_queue->data = malloc(sizeof(void*)*job_queue->capacity);
  // for (int i = 0; i < capacity; i++) {
  //   job_queue->data[i] = malloc(sizeof(void*));
  // }
  job_queue->front = 0;
  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  pthread_mutex_lock(&job_queue->mutex_general);
  while (job_queue->size > 0) {
    pthread_cond_wait(&job_queue->cond_destroy, &job_queue->mutex_general);
  }
  free(job_queue->data);
  //free(job_queue);
  pthread_mutex_unlock(&job_queue->mutex_general);
  //wake all job_queue_pop threads that are sleeping, so they can
  //return -1 and terminate
  pthread_cond_broadcast(&job_queue->cond_pop);
  return 0;
}

int job_queue_push(struct job_queue *job_queue, void *data) {
  pthread_mutex_lock(&job_queue->mutex_general);
  if (job_queue->size == job_queue->capacity) {
    pthread_cond_wait(&job_queue->cond_push,&job_queue->mutex_general);
  }
  pthread_mutex_unlock(&job_queue->mutex_general);
  // if thread has already been destroyed
  if (job_queue == NULL) {
    return -1;
  }
  //prevent another thread from running until the element is added,
  //since this is a critical section

  pthread_mutex_lock(&job_queue->mutex_general);
  job_queue->data[job_queue->front+job_queue->size] = data;
  job_queue->size++;
  pthread_mutex_unlock(&job_queue->mutex_general);
  //wake a sleeping pop thread as there is now an element it can pop
  pthread_cond_signal(&job_queue->cond_pop);
  
 return 0;

}

int job_queue_pop(struct job_queue *job_queue, void **data) {
  pthread_mutex_lock(&job_queue->mutex_general);
  if (job_queue->size == 0) {
    pthread_cond_wait(&job_queue->cond_pop, &job_queue->mutex_general);
  }
  pthread_mutex_unlock(&job_queue->mutex_general);
  //if thread has already been destroyed
  if (job_queue == NULL) {
    return -1;
  }
  pthread_mutex_lock(&job_queue->mutex_general);
  *data = job_queue->data[job_queue->front];
  job_queue->front = (job_queue->front + 1) % job_queue->capacity;
  job_queue->size--;
  pthread_mutex_unlock(&job_queue->mutex_general);
  if (job_queue->size == 0) {
    //if size is 0, wake the destroy thread if there is one since
    //we can now complete destroying the queue
    pthread_cond_signal(&job_queue->cond_destroy);
  }
  //wake a sleeping push thread 
  //if we are not destroying the queue since there will now be 
  //an available slot to push to
  else pthread_cond_signal(&job_queue->cond_push);
  return 0;

}