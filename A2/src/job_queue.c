#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "job_queue.h"

//conditions to make methods get blocked under certain conditions
pthread_cond_t cond_destroy = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_push =  PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pop =  PTHREAD_COND_INITIALIZER;

//mutex to hold methods locked while they should sleep
pthread_mutex_t mutex_destroy = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_push = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pop = PTHREAD_MUTEX_INITIALIZER;

//mutex to lock other threads out in critical sections
pthread_mutex_t mutex_general = PTHREAD_MUTEX_INITIALIZER;

int job_queue_init(struct job_queue *job_queue, int capacity) {
  job_queue->capacity = capacity;
  job_queue->size = 0;
  for (int i = 0; i < capacity;i++) {
    job_queue->data[i] = malloc(sizeof(void*));
  }
  job_queue->front = 0;
  job_queue->rear = 0;
  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  //probably not the best way to do this, but it should get the job done
  while(job_queue->size > 0) {
    pthread_cond_wait(&cond_destroy, &mutex_destroy);
  }
  //prevent job_queue_push from running until we have destroyed the queue
  //so we don't get added jobs in the middle
  pthread_mutex_lock(&mutex_general);
  for (int i = 0; i < job_queue->capacity; i++) {
    free(job_queue->data[i]);
  }
  free(job_queue);
  pthread_mutex_unlock(&mutex_push);
  pthread_cond_signal(&cond_pop);
  return 0;
}

int job_queue_push(struct job_queue *job_queue, void *data) {
  while(job_queue->size == job_queue->capacity) {
    pthread_cond_wait(&cond_push,&mutex_push);
  }
  //prevent another thread from running until the element is added,
  //since this is a critical section

  // if thread has already been destroyed
  if (job_queue == NULL) {
    return -1;
  }
  pthread_mutex_lock(&mutex_general);
  job_queue->data[job_queue->size] = data;
  job_queue->size++;
  job_queue->rear = (job_queue->rear + 1) % job_queue->capacity;
  pthread_mutex_unlock(&mutex_general);
  pthread_cond_signal(&cond_pop);
  
 return 0;

}

int job_queue_pop(struct job_queue *job_queue, void **data) {
  if (job_queue->size == 0) {
    pthread_cond_wait(&cond_pop, &mutex_pop);
  }
  //if thread has already been destroyed
  if (job_queue == NULL) {
    return -1;
  }
  pthread_mutex_lock(&mutex_general);
  *data = job_queue->data[job_queue->front];
  if (job_queue->size == 1) {
    job_queue->front = 0;
    job_queue->rear = 0;
    job_queue->size = 0;
  }
  else {
    job_queue->front = (job_queue->front + 1) % job_queue->capacity;
    job_queue->size--;
  }
  pthread_mutex_unlock(&mutex_general);
  pthread_cond_signal(&cond_push);
  return 0;

}
