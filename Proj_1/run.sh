#! /bin/bash
set -e
rm -f t.txt
for n in {1..60}; do
  cc -Wall -O2 -Dmax_n=$n count_sum_free_sets_v1.c
  rm -f a.txt
  ./a.out >a.txt
  grep seconds a.txt | tee -a t.txt
done
