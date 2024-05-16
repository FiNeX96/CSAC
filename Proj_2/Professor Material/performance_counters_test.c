#include <stdio.h>
#include "elapsed_time.h"
#include "linux_performance_counters.h"

static unsigned int f(void)
{
  unsigned int r,d;

  r = 0u;
  for(d = 1u;d <= 100000000u;d++)
    r += 100000000u / d;
  return r;
}

int main(int argc,char ** argv)
{
  unsigned long dt;
  unsigned int r;
  int i;

  (void)perf_counters_init();
  perf_counters[0].config = PERF_COUNT_HW_CPU_CYCLES;
  perf_counters[1].config = PERF_COUNT_HW_INSTRUCTIONS;
  perf_counters[2].config = PERF_COUNT_HW_REF_CPU_CYCLES;
  perf_counters[3].config = PERF_COUNT_HW_BRANCH_MISSES;
  if(perf_counters_open() < 0)
    return 1;
  printf("       f()  counter 0  counter 1  counter 2  counter 3 time stamp\n");
  printf("---------- ---------- ---------- ---------- ---------- ----------\n");
  for(i = 0;i < 4;i++)
  {
    if(perf_counters_start() < 0)
      return 1;
    dt = read_time_stamp_counter();
    r = f();
    dt = read_time_stamp_counter() - dt;
    if(perf_counters_stop() < 0)
      return 1;
    printf("%10u %10lu %10lu %10lu %10lu %10lu\n",r,perf_counters[0].value,perf_counters[1].value,perf_counters[2].value,perf_counters[3].value,dt);
  }
  printf("---------- ---------- ---------- ---------- ---------- ----------\n");
  perf_counters_close();
  return 0;
}
