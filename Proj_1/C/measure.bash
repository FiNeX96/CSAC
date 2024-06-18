#!/bin/bash
set -e

# Get the number of iterations and identifier from the user, set defaults if not provided
num_iterations=${1:-70}
cpu_name=${2:-""}

if [ -z "$cpu_name" ]; then
  # Get the CPU name and remove anything inside ( )
  cpu_name=$(lscpu | sed -nr '/Model name/ s/.*:\s*(.*) @ .*/\1/p' | sed -e 's/[(][^)]*[)]//g' -e 's/[()]//g')
fi

# Make it lower case, squeeze the spaces and turn them into underscores
cpu_name=$(echo "$cpu_name" | tr '[:upper:]' '[:lower:]' | tr -s ' ' | tr ' ' '_')

# Compile align_columns to be used later
cc -Wall -O2 -Wextra -Wundef align_columns.c -o align_columns

# Compile and run for v1_plain.c
for n in $(seq -w 01 $num_iterations); do
  cc -fcompare-debug-second -Wall -O2 -Dmax_n=$((1$n-100)) count_sum_free_sets_v1_plain.c

  line=$(./a.out | grep '^#' | cut -c3-)

  if [ "$n" -eq 01 ]; then
    echo "$line" > "${cpu_name}_v1_plain.txt"
  else
    echo "$line" >> "${cpu_name}_v1_plain.txt"
  fi

  rm -f a.out
done

./process_execution_time_file.bash "${cpu_name}_v1_plain.txt"

# Compile and run for v1_threads.c
for n in $(seq -w 01 $num_iterations); do
  cc -fcompare-debug-second -Wall -O2 -Dmax_n=$((1$n-100)) -pthread count_sum_free_sets_v1_threads.c

  line=$(./a.out | grep '^#' | cut -c3-)

  if [ "$n" -eq 01 ]; then
    echo "$line" > "${cpu_name}_v1_threads.txt"
  else
    echo "$line" >> "${cpu_name}_v1_threads.txt"
  fi

  rm -f a.out
done

./process_execution_time_file.bash "${cpu_name}_v1_threads.txt"

# Compile and run for v2_plain.c
for n in $(seq -w 01 $num_iterations); do
  cc -fcompare-debug-second -Wall -O2 -Dmax_n=$((1$n-100)) count_sum_free_sets_v2_plain.c

  line=$(./a.out | grep '^#' | cut -c3-)

  if [ "$n" -eq 01 ]; then
    echo "$line" > "${cpu_name}_v2_plain.txt"
  else
    echo "$line" >> "${cpu_name}_v2_plain.txt"
  fi

  rm -f a.out
done

./process_execution_time_file.bash "${cpu_name}_v2_plain.txt"

# Compile and run for v3_plain.c
for n in $(seq -w 01 $num_iterations); do
  cc -fcompare-debug-second -Wall -O2 -Dmax_n=$((1$n-100)) count_sum_free_sets_v3_plain.c

  line=$(./a.out | grep '^#' | cut -c3-)

  if [ "$n" -eq 01 ]; then
    echo "$line" > "${cpu_name}_v3_plain.txt"
  else
    echo "$line" >> "${cpu_name}_v3_plain.txt"
  fi

  rm -f a.out
done

./process_execution_time_file.bash "${cpu_name}_v3_plain.txt"
