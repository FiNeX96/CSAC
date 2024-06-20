package main

import (
	"flag"
	"fmt"
	"time"
)

var max_n = 60
var temp_max = 0
var a = make([]int, temp_max)
var sum_count = make([]int, 2*temp_max+1)
var count = make ([]int , temp_max+1)

func recurse(firstToTry int, setSize int ) {
	// try all ways to append one number to the current set
	for ; firstToTry <= temp_max; firstToTry++ {
		if sum_count[firstToTry] == 0 {
			// count it
			count[firstToTry]++
			// update the set elements
			a[setSize] = firstToTry
			// count new sums
			for i := 0; i <= setSize; i++ {
				sum_count[a[i]+firstToTry]++
			}
			// try one more
			recurse(firstToTry+1, setSize+1)
			// uncount sums
			for i := 0; i <= setSize; i++ {
				sum_count[a[i]+firstToTry]--
			}
		}
	}
}

func main () {
	// Get max_n from command-line argument
	max_n_flag := flag.Int("max_n", max_n, "The maximum number to process")
	flag.Parse()
	max_n = *max_n_flag

	count[0] = 1
	// count the rest
	for i := 1; i <= max_n ; i++ {
		temp_max = i
		a = make([]int, temp_max)
		sum_count = make([]int, 2*temp_max+1)
		count = make ([]int , temp_max+1)
		start_time := time.Now()
		recurse(1, 0)
		end_time := time.Since(start_time).Seconds()
		for i := 0; i <= temp_max; i++ {
			if (i > 0){
				count[i] += count[i-1]
			}
		}
		fmt.Printf("%d %d %.6f %d\n", i, count[i]+1, end_time, int(end_time))
	}
}
