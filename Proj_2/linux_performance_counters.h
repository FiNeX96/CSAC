//
// linux performance counters
//
// a return of 0 means success, a return of -1 means failure
//
// available counters --- see /usr/include/linux/perf_event.h for more, but using them requires changing perf_counters_open()
//   PERF_TYPE_HARDWARE
//     PERF_COUNT_HW_CPU_CYCLES
//     PERF_COUNT_HW_INSTRUCTIONS
//     PERF_COUNT_HW_CACHE_REFERENCES
//     PERF_COUNT_HW_CACHE_MISSES
//     PERF_COUNT_HW_BRANCH_INSTRUCTIONS
//     PERF_COUNT_HW_BRANCH_MISSES
//     PERF_COUNT_HW_BUS_CYCLES
//     PERF_COUNT_HW_STALLED_CYCLES_FRONTEND
//     PERF_COUNT_HW_STALLED_CYCLES_BACKEND
//     PERF_COUNT_HW_REF_CPU_CYCLES
// do "man perf_event_open" so see what each counter measures
//

#if !defined(__linux__) || !defined(__x86_64__)
# error "This code only works on a linux kernel on an Intel or AMD processor"
#endif

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>


//
// read tsc register
//

unsigned long read_time_stamp_counter(void)
{
  unsigned long t;

  asm volatile("rdtsc; shlq $32,%%rdx; orq %%rdx,%%rax" : "=a" (t) : : "rdx","cc");
  return t;
}

//
// linux performance counters global variables
//

#define max_perf_counters  4        // maximum number of performance counters we will read at the same time
#define perf_no_config     0xFFFFul // counter not configured

struct
{
  int fd;                // file descriptor
  unsigned long config;  // how the counter is configured
  unsigned long id;      //
  unsigned long value;   // the counter value
  unsigned long offset;  // the counter offset (empty measurement)
}
perf_counters[max_perf_counters];
int n_perf_counters;


//
// initialize the perf_counters[] array
//

int perf_counters_init(void)
{
  FILE *fp;
  int i,ok;

  //
  // initialize perf_counters data
  //
  for(i = 0;i < max_perf_counters;i++)
  {
    perf_counters[i].fd = -1;
    perf_counters[i].config = perf_no_config;
    perf_counters[i].offset = 0ul;
  }
  n_perf_counters = 0;
  //
  // check if we have free access to the performance events kernel infrastructure
  //
# define F "/proc/sys/kernel/perf_event_paranoid"
  fp = fopen(F,"rb");
  if(fp != NULL)
  {
    if(fscanf(fp,"%d",&i) == 1 && i > 2)
    {
      fprintf(stderr,"perf_counters_init: " F " should be 2 or less\n");
      fprintf(stderr,"  to correct this, as root, do \"echo 2 >" F "\"\n");
    }
    fclose(fp);
  }
#undef F
  //
  // check for turbo boost control
  //
  ok = -1; // unknown
# define F "/sys/devices/system/cpu/cpufreq/boost"
  if(ok < 0)
  {
    fp = fopen(F,"rb");
    if(fp != NULL)
    {
      if(fscanf(fp,"%d",&i) == 1)
        ok = (i == 1) ? 1 : 0;
      fclose(fp);
      fprintf(stderr,"perf_counters_init: turbo boost control in " F "\n");
    }
  }
# undef F
# define F "/sys/devices/system/cpu/intel_pstate/no_turbo"
  if(ok < 0)
  {
    fp = fopen(F,"rb");
    if(fp != NULL)
    {
      if(fscanf(fp,"%d",&i) == 1)
        ok = (i == 0) ? 1 : 0;
      fclose(fp);
      fprintf(stderr,"perf_counters_init: turbo boost control in " F "\n");
    }
  }
# undef F
  if(ok < 0)
    fprintf(stderr,"perf_counters_init: unable to determine turbo boost status\n");
  if(ok == 0)
    fprintf(stderr,"perf_counters_init: turbo boost is inactive\n");
  if(ok == 1)
    fprintf(stderr,"perf_counters_init: turbo boost is \e[31mactive\e[0m\n");
  //
  // done
  //
  return 0;
}


//
// close all performance counters file descriptors --- similar to perf_counters_init() but closes any open file descriptor
//

int perf_counters_close(void)
{
  int i;

  for(i = 0;i < max_perf_counters;i++)
  {
    if(perf_counters[i].fd >= 0)
      (void)close(perf_counters[i].fd);
    perf_counters[i].fd = -1;
    perf_counters[i].config = perf_no_config;
    perf_counters[i].offset = 0ul;
  }
  n_perf_counters = 0;
  return 0;
}


//
// activate the performance counters
//

int perf_counters_start(void)
{
  if(n_perf_counters <= 0 || n_perf_counters > max_perf_counters)
  {
    fprintf(stderr,"perf_counters_start: no counters\n");
    return -1;
  }
  if(ioctl(perf_counters[0].fd,PERF_EVENT_IOC_RESET,PERF_IOC_FLAG_GROUP) < 0)
  {
    perf_counters_close();
    fprintf(stderr,"perf_counters_start: ioctl IOC_RESET failed\n");
    return -1;
  }
  if(ioctl(perf_counters[0].fd,PERF_EVENT_IOC_ENABLE,PERF_IOC_FLAG_GROUP) < 0)
  {
    perf_counters_close();
    fprintf(stderr,"perf_counters_start: ioctl IOC_ENABLE failed\n");
    return -1;
  }
  return 0;
}


//
// deactivate the performance counters
//

int perf_counters_stop(void)
{
  struct
  {
    unsigned long nr;
    struct
    {
      unsigned long value;
      unsigned long id;
    }
    values[max_perf_counters];
  }
  data;
  int i,j,u[max_perf_counters];

  if(n_perf_counters <= 0 || n_perf_counters > max_perf_counters)
  {
    fprintf(stderr,"perf_counters_stop: no counters\n");
    return -1;
  }
  if(ioctl(perf_counters[0].fd,PERF_EVENT_IOC_DISABLE,PERF_IOC_FLAG_GROUP) < 0)
  {
    perf_counters_close();
    fprintf(stderr,"perf_counters_stop: ioctl IOC_DISABLE failed\n");
    return -1;
  }
  if(read(perf_counters[0].fd,(void *)&data,sizeof(data)) != (size_t)(1 + 2 * n_perf_counters) * sizeof(unsigned long))
  {
    perf_counters_close();
    fprintf(stderr,"perf_counters_stop: read failed\n");
    return -1;
  }
  if(data.nr != (unsigned long)n_perf_counters)
  {
    perf_counters_close();
    fprintf(stderr,"perf_counters_stop: unexpected number of counters\n");
    return -1;
  }
  for(i = 0;i < n_perf_counters;i++)
    u[i] = 0;
  for(i = 0;i < n_perf_counters;i++)
  {
    for(j = 0;j < n_perf_counters;j++)
      if(u[j] == 0 && data.values[i].id == perf_counters[j].id)
      {
        perf_counters[j].value = data.values[i].value;
        if(perf_counters[j].value >= perf_counters[j].offset)
          perf_counters[j].value -= perf_counters[j].offset;
        else
          perf_counters[j].value = 0ul;
        u[j] = 1;
        break;
      }
    if(j == n_perf_counters)
    {
      perf_counters_close();
      fprintf(stderr,"perf_counters_stop: failed to find a counter value\n");
      return -1;
    }
  }
  return 0;
}


//
// open the performance counters file descriptors
//

int perf_counters_open(void)
{
  unsigned long offset[max_perf_counters];
  struct perf_event_attr attr;
  int i,j,g;

  //
  // open
  //
  g = -1;
  for(i = 0;i < max_perf_counters && perf_counters[i].config != perf_no_config;i++)
  {
    for(j = 0;j < i && perf_counters[i].config != perf_counters[j].config;j++)
      ;
    if(j < i)
    {
      perf_counters_close();
      fprintf(stderr,"perf_counters_open: repeated counter (i=%d, j=%d)\n",i,j);
      return -1;
    }
    (void)memset(&attr,0,sizeof(attr));
    attr.size = sizeof(attr);
    attr.type = PERF_TYPE_HARDWARE;
    attr.config = perf_counters[i].config;
    attr.disabled = (g < 0) ? 1 : 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    perf_counters[i].fd = syscall(__NR_perf_event_open,&attr,0,-1,g,PERF_FLAG_FD_CLOEXEC);
    if(perf_counters[i].fd < 0)
    {
      perf_counters_close();
      fprintf(stderr,"perf_counters_open: perf_event_open syscall failed for i=%d\n",i);
      return -1;
    }
    if(ioctl(perf_counters[i].fd,PERF_EVENT_IOC_ID,&perf_counters[i].id) < 0)
    {
      perf_counters_close();
      fprintf(stderr,"perf_counters_open: ioctl IOC_ID failed for i=%d\n",i);
      return -1;
    }
    if(g < 0)
      g = perf_counters[i].fd;
  }
  if(i == 0)
  {
    fprintf(stderr,"perf_counters_open: no counters\n");
    return -1;
  }
  n_perf_counters = i;
  //
  // calibrate
  //
  for(i = 0;i < n_perf_counters;i++)
  {
    perf_counters[i].offset = 0ul;
    offset[i] = 0xFFFFFFFFFFFFFFFFul;
  }
  for(j = 0;j < 32;j++)
  {
    if(perf_counters_start() < 0 || perf_counters_stop() < 0)
    {
      perf_counters_close();
      return -1;
    }
    for(i = 0;i < n_perf_counters;i++)
      if(perf_counters[i].value < offset[i])
        offset[i] = perf_counters[i].value;
  }
  for(i = 0;i < n_perf_counters;i++)
    perf_counters[i].offset = offset[i];
  //
  // done
  //
  return 0;
}
