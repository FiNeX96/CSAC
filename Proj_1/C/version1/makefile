#
# makefile for the first assignment
#

clean:
	rm -f a.out v1_plain v1_threads

v1_plain:	count_sum_free_sets_v1_plain.c
	cc -Wall -O2 -Wextra -Wundef          count_sum_free_sets_v1_plain.c   -o v1_plain

v1_threads:	count_sum_free_sets_v1_threads.c
	cc -Wall -O2 -Wextra -Wundef -pthread count_sum_free_sets_v1_threads.c -o v1_threads
