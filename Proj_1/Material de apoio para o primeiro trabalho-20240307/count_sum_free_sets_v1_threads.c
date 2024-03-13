//
// Tomás Oliveira e Silva,  March 2024
//
// a set { a_1,a_2, ..., a_m } is sum-free if each of its elements is not the sum of any two elements
//   so, for any i, j, and k we must have a_k != a_i + a_j
// in this very simple program we count the number of subsets of { 1,2,...,n } that are sum-free
//   this is sequence A007865 of the OEIS (https://oeis.org/A007865)
//
// version 1 with threads --- multi-core using a pool of worker threads
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "elapsed_time.h"


//
// program configuration
// * adjust n_threads to match the number of cores of your processor
// * split_n should not be neither too small nor too large
//   8 appears to be reasonable, but for large max_n, 9 or even 10 may be better
//   this is so because at the end we will be waiting for the last thread to finish,
//   and a larger split_n will mean each thread will do smaller indivisible work chunks
//

#ifndef max_n
# define max_n           40
#endif
#ifndef n_threads
# define n_threads        2
#endif
#ifndef split_n
# define split_n          8
#endif


//
// global computation state
//

typedef struct
{
  int a[max_n];                   // the set elements
  int sum_count[2 * max_n + 1];   // the number of times a_i+a_j has been observed
  unsigned long count[1 + max_n]; // the number of subsets (indexed by its last element)
}
state_t;

static volatile state_t state;


//
// pool of worker threads
//

typedef enum
{
  wait_action,
  work_action,
  exit_action
}
work_action_t;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // critical section access control
static pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;  // to wait for events
static pthread_t tid[n_threads];                         // thread identifiers
static volatile work_action_t what_to_do;                // what threads have to do
static volatile struct
{
  int first_to_try;             // -1 means consumed
  int set_size;
  int a[max_n];
  int sum_count[2 * max_n + 1];
  int total_work_items;         // count of the number of work items
}
work_item;                                               // work item to be passed to the thread

#define LOCK()    do if(pthread_mutex_lock(&lock)      != 0) { fprintf(stderr,"pthread_mutex_lock() failed\n");     exit(1); } while(0)
#define UNLOCK()  do if(pthread_mutex_unlock(&lock)    != 0) { fprintf(stderr,"pthread_mutex_unlock() failed\n");   exit(1); } while(0)
#define SIGNAL()  do if(pthread_cond_broadcast(&cond)  != 0) { fprintf(stderr,"pthread_cond_broadcast() failed\n"); exit(1); } while(0)
#define WAIT()    do if(pthread_cond_wait(&cond,&lock) != 0) { fprintf(stderr,"pthread_cond_wait() failed\n");      exit(1); } while(0)


//
// thread recursion and worker threads
//

static void thread_recurse(state_t *s,int first_to_try,int set_size)
{
  // try all ways to append one number to the current set
  for(;first_to_try <= max_n;first_to_try++)
    if(s->sum_count[first_to_try] == 0)
    {
      // count it
      s->count[first_to_try]++;
      // update the set elements
      s->a[set_size] = first_to_try;
      // count new sums
      for(int i = 0;i <= set_size;i++)
        s->sum_count[s->a[i] + first_to_try]++;
      // try one more
      thread_recurse(s,first_to_try + 1,set_size + 1);
      // uncount sums
      for(int i = 0;i <= set_size;i++)
        s->sum_count[s->a[i] + first_to_try]--;
    }
}

static void *worker_thread(void *unused)
{
  int i,first_to_try,set_size;
  state_t thread_state;

  // make the compiler happy (unused is NULL so this is harmless but "uses" the variable)
  if(unused != NULL)
    exit(1);
  // initialize thread counters
  for(i = 0;i < 1 + max_n;i++)
    thread_state.count[i] = 0ul;
  // do work
  for(;;)
  {
    first_to_try = set_size = -1;
    // see if there is work to do
    LOCK();
    if(what_to_do == wait_action)
      WAIT(); // wait if there is nothing to do
    i = 0; // the default is nothing to do after UNLOCK()
    if(what_to_do == exit_action)
    {
      // update global counters
      for(i = 0;i < 1 + max_n;i++)
        state.count[i] += thread_state.count[i];
      // terminate thread after UNLOCK()
      i = -1;
    }
    else if(what_to_do == work_action)
    {
      // get the first_to_try and set_size values
      first_to_try = work_item.first_to_try;
      set_size = work_item.set_size;
      // get the set
      for(i = 0;i < set_size;i++)
        thread_state.a[i] = work_item.a[i];
      // get the observed sums
      for(i = 0;i < 2 * max_n + 1;i++)
        thread_state.sum_count[i] = work_item.sum_count[i];
      // wake everybody
      what_to_do = wait_action;
      work_item.first_to_try = -1;
      // work after UNLOCK()
      SIGNAL();
      i = 1;
    }
    UNLOCK();
    // act
    if(i < 0)
      return NULL;
    if(i > 0 && first_to_try > 0)
      thread_recurse(&thread_state,first_to_try,set_size);
  }
}


//
// main recursion
//

static void recurse(int first_to_try,int set_size)
{
  if(first_to_try >= split_n)
  {
    int i;

    do
    {
      i = 1;
      LOCK();
      if(work_item.first_to_try >= 0)
        WAIT(); // wait until some thread fetches the current work item
      if(what_to_do == wait_action)
      {
        what_to_do = work_action;
        // set the first_to_try and set_size values
        work_item.first_to_try = first_to_try;
        work_item.set_size = set_size;
        // set the set
        for(i = 0;i < set_size;i++)
          work_item.a[i] = state.a[i];
        // set the observed sums
        for(i = 0;i < 2 * max_n + 1;i++)
          work_item.sum_count[i] = state.sum_count[i];
        // wake everybody
        work_item.total_work_items++;
        SIGNAL();
        i = 0;
      }
      UNLOCK();
    }
    while(i != 0);
    return;
  }
  // try all ways to append one number to the current set
  for(;first_to_try <= max_n;first_to_try++)
    if(state.sum_count[first_to_try] == 0)
    {
      // count it
      state.count[first_to_try]++;
      // update the set elements
      state.a[set_size] = first_to_try;
      // count new sums
      for(int i = 0;i <= set_size;i++)
        state.sum_count[state.a[i] + first_to_try]++;
      // try one more
      recurse(first_to_try + 1,set_size + 1);
      // uncount sums
      for(int i = 0;i <= set_size;i++)
        state.sum_count[state.a[i] + first_to_try]--;
    }
}


//
// main program
//

int main(void)
{
  int i;

  // count the empty set
  state.count[0] = 1ul;
  // count the rest
  start_time_measurement(5);
  what_to_do = wait_action;
  work_item.first_to_try = -1;
  work_item.total_work_items = 0;
  for(i = 0;i < n_threads;i++)
    if(pthread_create(&tid[i],NULL,worker_thread,NULL) != 0)
    {
      fprintf(stderr,"pthread_create() failed\n");
      exit(1);
    }
  recurse(1,0);
  LOCK();
  if(work_item.first_to_try >= 0)
    WAIT(); // wait until some thread fetches the last work item
  what_to_do = exit_action;
  SIGNAL();
  UNLOCK();
  for(i = 0;i < n_threads;i++)
    if(pthread_join(tid[i],NULL) != 0)
    {
      fprintf(stderr,"pthread_join() failed\n");
      exit(1);
    }
  time_measurement();
  // report
  for(i = 0;i <= max_n;i++)
  {
    if(i > 0)
      state.count[i] += state.count[i - 1];
    printf("%d %lu\n",i,state.count[i]);
  }
  printf("# %d %lu %.6f %ld\n",max_n,state.count[max_n],cpu_time_delta,(long)wall_time_delta);
  printf("%d work items\n",work_item.total_work_items);
  return 0;
}
