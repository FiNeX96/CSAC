//
// Tomás Oliveira e Silva,  February 2024
//
// a set { a_1,a_2, ..., a_m } is sum-free if each of its elements is not the sum of any two elements
//   so, for any i, j, and k we must have a_k != a_i + a_j
// in this very simple program we count the number of subsets of { 1,2,...,n } that are sum-free
//   this is sequence A007865 of the OEIS (https://oeis.org/A007865)
//
// version 2 --- with bit tricks, cannot break the 64-bit barrie
//

#include <stdio.h>
#include "elapsed_time.h"

#ifndef max_n
# define max_n 60
#endif

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

int main(void)
{
  // count the empty set
  count[0] = 1ul;
  // count the rest
  start_time_measurement(5);
  recurse(0ul,(1ul << max_n) - 1ul);
  time_measurement();
  // report
  for(int i = 0;i <= max_n;i++)
  {
    if(i > 0)
      count[i] += count[i - 1];
    printf("%d %lu\n",i,count[i]);
  }
  printf("# %d %lu %.6f %ld\n",max_n,count[max_n],cpu_time_delta,(long)wall_time_delta);
  return 0;
}
