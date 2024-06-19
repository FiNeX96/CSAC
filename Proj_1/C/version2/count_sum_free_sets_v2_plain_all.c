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
#include "elapsed_time.h"


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

static unsigned long count[1 + max_n]; // the number of subsets (indexed by its last element)

#if defined(__x86_64__)

static int first_non_zero_bit(unsigned long mask)
{
  long result;

  asm volatile("tzcntq\t%[data],%[result]"
    : [result] "=r" (result)
    : [data]   "r"  (mask)
    : "cc"
  );
  return (int)result;
}

#else

#define first_non_zero_bit(mask)  (int)__builtin_ctzl((unsigned long)(mask))

#endif

//  Clocks for timing
struct timespec start, end;

//
// main recursion
//

static void recurse(unsigned long set_mask,unsigned long negated_sum_mask)
{
  unsigned long new_set_mask,new_negated_sum_mask;
  int bit;

  // try all ways to append one number to the current set
  while(negated_sum_mask != 0ul)
  {
    // get the smallest possible next number
    bit = first_non_zero_bit(negated_sum_mask);
    // count it
    count[1 + bit]++;
    // get rid of the number
    negated_sum_mask &= negated_sum_mask - 1ul;
    // update the set
    new_set_mask  = set_mask | (1ul << bit);
    // update the sums
    new_negated_sum_mask = negated_sum_mask & ~(new_set_mask << (bit + 1));
    // recurse
    recurse(new_set_mask,new_negated_sum_mask);
  }
}


//
// main program
//

void run_solver()
{
  // count the empty set
  count[0] = 1ul;
  // count the rest
  clock_t start = clock(), diff;
  recurse(0ul,(1ul << temp_n) - 1ul);
  diff = clock() - start;
  // report
  for(int i = 0;i <= temp_n;i++)
  {
    if(i > 0)
      count[i] += count[i - 1];
    //printf("%d %lu\n",i,count[i]);
  }
  float sec = ((float)diff * 1000 / CLOCKS_PER_SEC) / 1000;
  printf("%2d %12lu %11.6f %4d\n", temp_n, count[temp_n], sec, (int)sec);

}

int main(void)
{
  printf("#n        count         cpu wall\n#- ------------ ----------- ----\n");

  for(int j = 0; j <= max_n; j++)
  {
    memset(count, 0, sizeof count);
    temp_n = j;
    run_solver();
  }
}