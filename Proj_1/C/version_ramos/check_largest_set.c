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
# define max_n 50
#endif

int temp_n = 0;

//
// global computation state
//

//  This will always make the size of these structures occupy a lot of uneeded space for small n
typedef struct
{
  int a[max_n];                   // the set elements
  unsigned long count[1 + max_n]; // the number of subsets (indexed by its last element)
}
state_t;

static state_t state;

//  Clocks for timing
struct timespec start, end;


//
// main recursion
//

static void recursive(int number_to_try, int set_size) {

  //  The first number can always enter

  //printf("TRY %d\n", number_to_try);
  // try all ways to append one number to the current set
  for(;number_to_try <= temp_n; number_to_try++) {
    bool canAdd = true;

    // count new sums
    for(int numIdxToSum = 0; numIdxToSum < set_size ; numIdxToSum++) {

      for(int numIdxToCheck = numIdxToSum + 1; numIdxToCheck < set_size; numIdxToCheck++) {

        //printf("Checking if %d + %d = %d\n", state.a[numIdxToSum], state.a[numIdxToCheck], number_to_try);

        if (state.a[numIdxToSum] + state.a[numIdxToCheck] ==  number_to_try) {
          canAdd = false;

          //  Exit both loops
          numIdxToSum = set_size;
          //printf("IGNORED %d\n", number_to_try);
          continue;
        }
      }
    }

    if (canAdd) {
      // update the set elements
      // count it
      //printf("ADDED %d\n", number_to_try);
      state.count[number_to_try]++;
      state.a[set_size] = number_to_try;
      set_size++;
    }
  }
}


//
// main program
//

void run_solver() {
  
  // count the empty set
  state.count[0] = 1ul;

  // count the rest
  clock_t start = clock(), diff;
  recursive(1,0);
  diff = clock() - start;

  // report
  for(int i = 0;i <= temp_n;i++) {
    if(i > 0) {
      state.count[i] += state.count[i - 1];
    }
  }

  float sec = ((float)diff * 1000 / CLOCKS_PER_SEC) / 1000;

  printf("# %d %lu %6.3e\n",temp_n,state.count[temp_n],sec);
}

int main(void) {
  for(int i = 1;i <= max_n;i++) {
    memset(state.a, 0, sizeof state.a);
    memset(state.count, 0, sizeof state.count);
    temp_n = i;
    run_solver();

    printf("{");
    for(int j = 0; j < temp_n; j++) {
        printf(" %d,", state.a[j]);
    }
    printf("}\n");
  }
  return 0;
}