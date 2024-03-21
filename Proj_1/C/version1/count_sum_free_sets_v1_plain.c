//
// Tomás Oliveira e Silva,  February 2024
//
// a set { a_1,a_2, ..., a_m } is sum-free if each of its elements is not the sum of any two elements
//   so, for any i, j, and k we must have a_k != a_i + a_j
// in this very simple program we count the number of subsets of { 1,2,...,n } that are sum-free
//   this is sequence A007865 of the OEIS (https://oeis.org/A007865)
//
// version 1 plain --- no parallelization harness
//

#include <stdio.h>
#include "elapsed_time.h"


//
// program configuration
//

#ifndef max_n
# define max_n 50
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

static state_t state;


//
// main recursion
//

static void recurse(int first_to_try,int set_size)
{
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
  // count the empty set
  state.count[0] = 1ul;
  // count the rest
  start_time_measurement(5);
  recurse(1,0);
  time_measurement();
  // report
  for(int i = 0;i <= max_n;i++)
  {
    if(i > 0)
      state.count[i] += state.count[i - 1];
    printf("%d %lu\n",i,state.count[i]);
  }
  printf("# %d %lu %.6f %ld\n",max_n,state.count[max_n],cpu_time_delta,(long)wall_time_delta);
    printf("{");
    for(int j = 0; j < max_n; j++) {
        printf(" %d,", state.a[j]);
    }
    printf("}\n");
  return 0;
}
