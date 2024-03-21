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
#include <string.h> 
#include <time.h>
#include <stdbool.h> 


//
// program configuration
//

#ifndef max_n
# define max_n 60
#endif

int temp_n = 0;

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

//  Clocks for timing
struct timespec start, end;

//
// main recursion
//

static void recurse(int first_to_try,int set_size)
{
  // try all ways to append one number to the current set
  for(;first_to_try <= temp_n;first_to_try++)
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

void run_solver()
{
  // count the empty set
  state.count[0] = 1ul;
  // count the rest
  clock_t start = clock(), diff;
  recurse(1,0);
  diff = clock() - start;
  // report
  for(int i = 0;i <= temp_n;i++)
  {
    if(i > 0)
      state.count[i] += state.count[i - 1];
    //printf("%d %lu\n",i,state.count[i]);
  }
  
  float sec = ((float)diff * 1000 / CLOCKS_PER_SEC) / 1000;

  printf("%2d %12lu %11.6f %4d\n", temp_n, state.count[temp_n], sec, (int)sec);

}

int main(void)
{
  printf("#n        count         cpu wall\n#- ------------ ----------- ----\n");

  for(int j = 0; j <= max_n; j++)
  {
    memset(state.a, 0, sizeof state.a);
    memset(state.count, 0, sizeof state.count);
    temp_n = j;
    run_solver();
  }
}