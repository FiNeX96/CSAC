//
// Tomás Oliveira e Silva,  March 2024
//
// code to measure the elapsed time used by a program fragment
//
// #define USE_GETRUSAGE to force the tilization of the getrusage() function
// #define USE_CLOCK to force the utilization of the clock() function
//
// use as follows:
//
//   start_time_measurement(3);
//   // put your code to be time measured here
//   time_measurement();
//   printf("total CPU time: %.6f seconds\n",cpu_time_delta);
//   printf("elapsed time: %ld seconds\n",(long)wall_time_delta);
//


//
// high-level functions
//

#include <time.h>
double cpu_time(void);

static double initial_cpu_time,final_cpu_time,cpu_time_delta;
static time_t initial_wall_time,final_wall_time,wall_time_delta;

void start_time_measurement(int warmup_seconds)
{
  if(warmup_seconds < 1)
    warmup_seconds = 1;
  if(warmup_seconds > 5)
    warmup_seconds = 5; // 5 seconds may not be enough to heap the processor up
  initial_wall_time = time(NULL);
  while(warmup_seconds > 0)
  {
    time_t t = time(NULL);
    if(t != initial_wall_time)
    {
      initial_wall_time = t;
      warmup_seconds--;
    }
  }
  initial_cpu_time = cpu_time();
}

static void time_measurement(void)
{
  final_wall_time = time(NULL);
  final_cpu_time = cpu_time();
  wall_time_delta = final_wall_time - initial_wall_time;
  cpu_time_delta = final_cpu_time - initial_cpu_time;
}


//
// low-level function
//

// force the utilization of the getrusage() function
#if defined(USE_GETRUSAGE)
#pragma message "using getrusage()"

#include <sys/time.h>
#include <sys/resource.h>

double cpu_time(void)
{
  struct rusage r;

  if(getrusage(RUSAGE_SELF,&r) != 0)
    return -1.0;
  return (double)(r.ru_utime.tv_sec + r.ru_stime.tv_sec) + 1.0e-6 * (double)(r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

// force the utilization of the clock() function
#elif defined(USE_CLOCK)
#pragma message "using clock()"

double cpu_time(void)
{
  return (double)clock() / (double)CLOCKS_PER_SEC;
}

// GNU/Linux and MacOS code
#elif defined(__linux__) || defined(__APPLE__)
#pragma message "using clock_gettime()"

double cpu_time(void)
{
  struct timespec current_time;

  if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&current_time) != 0)  // the first argument could also be CLOCK_REALTIME
    return -1.0; // clock_gettime() failed!!!
  return (double)current_time.tv_sec + 1.0e-9 * (double)current_time.tv_nsec;
}

// Microsoft Windows code
#elif defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#pragma message "using QueryPerformanceCounter()"

#include <windows.h>

double cpu_time(void)
{
  static LARGE_INTEGER frequency;
  static int first_time = 1;
  LARGE_INTEGER current_time;

  if(first_time != 0)
  {
    QueryPerformanceFrequency(&frequency);
    first_time = 0;
  }
  QueryPerformanceCounter(&current_time);
  return (double)current_time.QuadPart / (double)frequency.QuadPart;
}

#endif
