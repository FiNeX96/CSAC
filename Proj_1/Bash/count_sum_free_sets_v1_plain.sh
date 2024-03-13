#!/bin/bash

MAX_N=40
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
            a[set_size]=$first_try
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

    count=($(for ((a=0; a<=temp_max; a++)); do echo 0; done))
    count[0]=1
    sum_count=($(for ((b=0; b<=temp_max*2; b++)); do echo 0; done))
    a=($(for ((c=0; c<=temp_max; c++)); do echo 0; done))

    start_time=$(($(date +%s%N)/1000000))
    recursive 1 0 $temp_max
    end_time=$(($(date +%s%N)/1000000))
    total_time=$(echo "$end_time - $start_time" | bc)

    # print the result
    for ((i=0; i<=temp_max; i++)); do
        if ((i > 0)); then
            ((count[i] += count[i-1]))
        fi
    done

    printf "# %d %d %6.3e\n" "$temp_max" "${count[temp_max]}" "$(($total_time/1000))"

}

for ((n=1; n<=MAX_N; n++)); do
    run_solver $n
done
