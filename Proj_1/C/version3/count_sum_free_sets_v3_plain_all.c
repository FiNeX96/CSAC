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

//  Clocks for timing
struct timespec start, end;

//
// main recursion
//


//
// main program
//

void run_solver()
{
  // count the empty set
  count[0] = 1ul;
  // count the rest
  state_t *sp,state,stack[1 + max_n];
  int b;

  clock_t start = clock(), diff;
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
  if((state.set_element_value += b) > temp_n)
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
  diff = clock() - start;
  // done, report
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