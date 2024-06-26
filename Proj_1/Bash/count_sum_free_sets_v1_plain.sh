#!/bin/bash

MAX_N=${1:-35}
a=()
sum_count=()
count=()

recursive() {
    local first_try=$1
    local set_size=$2
    local temp_max=$3

    while ((first_try <= temp_max)); do
        if ((sum_count[first_try] == 0)); then
            # count it
            ((count[first_try]++))
            # update the set elements
            ((a[set_size]=first_try))
            # count new sums
            for ((m=0; m<=set_size; m++)); do
                ((sum_count[first_try + a[m]]++))
            done
            # try one more
            recursive $((first_try + 1)) $((set_size + 1)) $temp_max
            # uncount sums
            for ((j=0; j<=set_size; j++)); do
                ((sum_count[first_try + a[j]]--))
            done
        fi

        # update vars
        ((first_try++))
    done
}

run_solver() {
    local temp_max=$1

    for ((b=0;b<$temp_max;b++)); do count[$b]=0; done
    count[0]=1
    for ((c=0;c<=($temp_max*2);c++)); do sum_count[$c]=0; done
    for ((d=0;d<=$temp_max;d++)); do a[$d]=0; done

    start_time=$(date +%s.%N)
    recursive 1 0 $temp_max
    end_time=$(date +%s.%N)
    total_time=$(echo "$end_time - $start_time" | bc -l)
    total_time_int=$(printf "%.0f" "$total_time")

    printf "%d %d %.6f %d\n" "$temp_max" "${count[temp_max]}" "$total_time" "$total_time_int"
}

for ((n=1; n<=MAX_N; n++)); do
    run_solver $n
done
