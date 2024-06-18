#! /bin/bash

CCFLAGS="-O3 -march=native"

if [ ! -e matrix_multiplication_L1.txt ]; then
  cc -Wall $CCFLAGS -DLAYOUT=1 matrix_multiplication.c
  taskset -c 0 ./a.out > matrix_multiplication_L1.txt
  rm -f a.out
fi
if [ ! -e matrix_multiplication_L2.txt ]; then
  cc -Wall $CCFLAGS -DLAYOUT=2 matrix_multiplication.c
  taskset -c 0 ./a.out > matrix_multiplication_L2.txt
  rm -f a.out
fi
if [ ! -e matrix_multiplication_L3.txt ]; then
  cc -Wall $CCFLAGS -DLAYOUT=3 matrix_multiplication.c
  taskset -c 0 ./a.out > matrix_multiplication_L3.txt
  rm -f a.out
fi
