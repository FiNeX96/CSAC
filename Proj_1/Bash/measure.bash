#!/bin/bash
set -e

# Check if cpu_name is provided
if [ -z "$1" ]; then
    echo "Usage: $0 \"<cpu_name>\" [num_iterations]"
    exit 1
fi

# Get the number of iterations and identifier from the user, set defaults if not provided
cpu_name=$1
num_iterations=${2:-35}

# Make it lower case, squeeze the spaces and turn them into underscores
cpu_name=$(echo "$cpu_name" | tr '[:upper:]' '[:lower:]' | tr -s ' ' | tr ' ' '_')

# Compile align_columns to be used later
cc -Wall -O2 -Wextra -Wundef ../align_columns.c -o align_columns

./count_sum_free_sets_v1_plain.sh ${num_iterations} > "../exec_times/Bash/${cpu_name}_v1_bash.txt"

./process_execution_time_file.bash "../exec_times/Bash/${cpu_name}_v1_bash.txt"
