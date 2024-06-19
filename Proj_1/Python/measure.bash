#!/bin/bash
set -e

# Get the number of iterations and identifier from the user, set defaults if not provided
cpu_name=${1:-""}
num_iterations=${2:-50}

if [ -z "$cpu_name" ]; then
  # Get the CPU name and remove anything inside ( )
  cpu_name=$(lscpu | sed -nr '/Model name/ s/.*:\s*(.*) @ .*/\1/p' | sed -e 's/[(][^)]*[)]//g' -e 's/[()]//g')
fi

# Make it lower case, squeeze the spaces and turn them into underscores
cpu_name=$(echo "$cpu_name" | tr '[:upper:]' '[:lower:]' | tr -s ' ' | tr ' ' '_')

# Compile align_columns to be used later
cc -Wall -O2 -Wextra -Wundef ../align_columns.c -o align_columns

python count_sum_free_sets.py ${num_iterations} > "exec_times/${cpu_name}_v1_python.txt"

./process_execution_time_file.bash "exec_times/${cpu_name}_v1_python.txt"
