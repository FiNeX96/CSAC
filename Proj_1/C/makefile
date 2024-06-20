#
# makefile for the first assignment
#

clean:
	rm -f a.out align_columns v1_plain v1_threads v1_plain_all v2_plain v3_plain

align_columns:	align_columns.c
	cc -Wall -O2 -Wextra -Wundef          align_columns.c   -o align_columns

v1_plain:	count_sum_free_sets_v1_plain.c
	cc -Wall -O2 -Wextra -Wundef          count_sum_free_sets_v1_plain.c   -o v1_plain

v1_threads:	count_sum_free_sets_v1_threads.c
	cc -Wall -O2 -Wextra -Wundef -pthread count_sum_free_sets_v1_threads.c -o v1_threads

v1_plain_all:	count_sum_free_sets_v1_plain_all.c
	cc -Wall -O2 -Wextra -Wundef          count_sum_free_sets_v1_plain_all.c   -o v1_plain_all

v2_plain:	count_sum_free_sets_v2_plain.c
	cc -Wall -O2 -Wextra -Wundef          count_sum_free_sets_v2_plain.c   -o v2_plain

v3_plain:	count_sum_free_sets_v3_plain.c
	cc -Wall -O2 -Wextra -Wundef          count_sum_free_sets_v3_plain.c   -o v3_plain