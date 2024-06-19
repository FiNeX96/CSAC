//
// Tomás Oliveira e Silva,  February 2024
//
// a set { a_1,a_2, ..., a_m } is sum-free if each of its elements is not the sum of any two elements
//   so, for any i, j, and k we must have a_k != a_i + a_j
// in this very simple program we count the number of subsets of { 1,2,...,n } that are sum-free
//   this is sequence A007865 of the OEIS (https://oeis.org/A007865)
//
// version 3 --- with bit tricks, cannot break the 128-bit barrier
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

typedef struct
{
  unsigned long set;         // the bit-mask of the first elements of the set
  unsigned long set_element; // the bit-mask of the new element to be inserted into the set (may be zero)
  unsigned long sums;        // the bit-mask of the observed sums
  int set_element_value;     // the number of the new element to be inserted into the set
}
state_t;

int main(void)
{
  state_t *sp,state,stack[1 + max_n];
  int b;

  start_time_measurement(5);
  // count the empty set
  count[0] = 1ul;
  // prepare to try all possibilities
  state.set = 0ul;             // nothing yet
  state.set_element = 1ul;     // the number one in bit 0
  state.sums = 0ul;            // nothing yet
  state.set_element_value = 1; // the number one
  sp = &stack[0];
loop:
  // find the position of the first zero in the sums mask
  // the tzcnt instruction returns 64 when its argument is zero, so it does the right thing
  // example (set, set bits, set elements, sums) of what happen when all bits of state.sums are ones:
  //   DC85237200008020 1101110010000101001000110111001000000000000000001000000000100000 [5,15,33,36,37,38,40,41,45,48,50,55,58,59,60,62,63] all sums 63..126
  // can only happen when state.set_element_value >= 64, so no problem
#if 0
  if(~state.sums == 0ul)
    goto back;
#endif
  b = first_non_zero_bit(~state.sums);
  // shift state
  if((state.set_element_value += b) > max_n)
    goto back;
  state.set_element <<= b;
  state.sums = (state.sums >> b) | 1ul; // shift and set the least significant bit
  // count
  count[state.set_element_value]++;
  // save current state
  *sp++ = state;
  // new state
  state.set |= state.set_element;
  state.sums = (state.sums >> 1) | state.set;
  state.set_element <<= 1;
  state.set_element_value++;
  goto loop;
back:
  // backtrack
  if(sp > &stack[0])
  {
    state = *--sp;
    goto loop;
  }
  time_measurement();
  // done, report
  for(int i = 0;i <= max_n;i++)
  {
    if(i > 0)
      count[i] += count[i - 1];
    printf("%d %lu\n",i,count[i]);
  }
  printf("# %d %lu %.6f %ld\n",max_n,count[max_n],cpu_time_delta,(long)wall_time_delta);
  return 0;
}
