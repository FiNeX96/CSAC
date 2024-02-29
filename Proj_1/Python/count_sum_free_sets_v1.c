//
// Tomás Oliveira e Silva,  February 2024
//
// a set { a_1,a_2, ..., a_m } is sum-free if each of its elements is not the sum of any two elements
//   so, for any i, j, and k we must have a_k != a_i + a_j
// in this very simple program we count the number of subsets of { 1,2,...,n } that are sum-free
//   this is sequence A007865 of the OEIS (https://oeis.org/A007865)
//
// results up to max_n=60 obtained on a Intel Core i7-5500U CPU @ 2.40GHz using the following bash script
//
//   #! /bin/bash
//   set -e
//   rm -f t.txt
//   for n in {1..60}; do
//     cc -Wall -O2 -Dmax_n=$n count_sum_free_sets_v1.c
//     rm -f a.txt
//     ./a.out >a.txt
//     grep seconds a.txt | tee -a t.txt
//   done
//
// the results (pasted together and with header and separator lines, the cpu time appears to be close to 2e-8 * max_n * 1.383^max_n)
//
//   max_n           count    cpu time
//   ----- --------------- -----------
//       0               1            
//       1               2 4.13675e-04
//       2               3 4.62508e-04
//       3               6 4.10646e-04
//       4               9 3.83441e-04
//       5              16 4.61973e-04
//       6              24 4.12209e-04
//       7              42 4.48445e-04
//       8              61 5.10625e-04
//       9             108 5.57181e-04
//      10             151 4.35404e-04
//      11             253 4.86843e-04
//      12             369 4.68331e-04
//      13             607 4.78450e-04
//      14             847 4.30296e-04
//      15            1400 4.84356e-04
//      16            1954 5.43574e-04
//      17            3139 5.29199e-04
//      18            4398 5.52754e-04
//      19            6976 5.35336e-04
//      20            9583 6.98204e-04
//      21           15456 8.08213e-04
//      22           20982 8.42837e-04
//      23           32816 1.37950e-03
//      24           45417 1.46047e-03
//      25           70109 2.42469e-03
//      26           94499 2.60316e-03
//      27          148234 3.49865e-03
//      28          200768 5.11648e-03
//      29          308213 6.85274e-03
//      30          415543 9.83977e-03
//      31          634270 1.42683e-02
//      32          849877 1.96244e-02
//      33         1311244 2.95641e-02
//      34         1739022 3.92271e-02
//      35         2630061 5.80251e-02
//      36         3540355 7.81197e-02
//      37         5344961 1.20397e-01
//      38         7051789 1.59439e-01
//      39        10747207 2.39623e-01
//      40        14158720 3.25595e-01
//      41        21295570 4.88584e-01
//      42        28188520 6.59618e-01
//      43        42283059 9.84999e-01
//      44        55560183 1.32055e+00
//      45        83902379 1.99626e+00
//      46       109416660 2.65776e+00
//      47       163758608 3.98207e+00
//      48       215578633 5.39479e+00
//      49       322546325 8.03843e+00
//      50       417931547 1.05954e+01
//      51       630427082 1.59713e+01
//      52       819313304 2.11012e+01
//      53      1225749154 3.17117e+01
//      54      1594365915 4.18902e+01
//      55      2379621002 6.37628e+01
//      56      3079772605 8.41604e+01
//      57      4644363603 1.25294e+02
//      58      5978202515 1.64242e+02
//      59      8958100404 2.50436e+02
//      60     11592193582 3.26584e+02
//   ----- --------------- -----------
//      88 126033894384609     50 days (world record, cpu time estimate using this program)
//   ----- --------------- -----------
//

#include <stdio.h>
#include "elapsed_time.h"

#ifndef max_n
#define max_n 40
#endif

static int a[max_n];                   // the set elements
static int sum_count[2 * max_n + 1];   // the number of times a_i+a_j has been observed
static unsigned long count[1 + max_n]; // the number of subsets (indexed by its last element)

static void recurse(int first_to_try,int set_size) {

  // try all ways to append one number to the current set
  for(;first_to_try <= max_n;first_to_try++)

    if(sum_count[first_to_try] == 0) {
      // count it
      count[first_to_try]++;

      // update the set elements
      a[set_size] = first_to_try;

      // count new sums
      for(int i = 0;i <= set_size;i++)
        sum_count[a[i] + first_to_try]++;

      // try one more
      recurse(first_to_try + 1,set_size + 1);

      // uncount sums
      for(int i = 0;i <= set_size;i++)
        sum_count[a[i] + first_to_try]--;

    }

}

int main(void) {

  // count the empty set
  count[0] = 1ul;

  // count the rest
  recurse(1,0);

  // report
  for(int i = 0;i <= max_n;i++) {
    
    if(i > 0)
      count[i] += count[i - 1];

    printf("%2d %lu\n",i,count[i]);
  
  }

  printf("# %2d %.5e seconds\n",max_n,cpu_time());

}
