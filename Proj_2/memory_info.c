//
// Tomás Oliveira e Silva,  March 2024
//
// measure the bandwidth and latency of memory accesses (affected by TLB updates...)
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#ifdef MAP_HUGETLB
# include <linux/mman.h>
#endif
#include "elapsed_time.h"


//
// Intel/AMD specific stuff (cpuid)
//
// RTFM
//

#ifdef __x86_64__

typedef struct
{
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
}
cpuid_data_t;

static void cpuid(unsigned int eax,unsigned int ecx,cpuid_data_t *data)
{
  unsigned int ebx,edx;

  asm volatile("cpuid"
    : "+a" (eax), // input and output in eax
      "+c" (ecx), // input and output in ecx
      "=b" (ebx), // output in ebx
      "=d" (edx)  // output in edx
  );
  data->eax = eax;
  data->ebx = ebx;
  data->ecx = ecx;
  data->edx = edx;
}

static void cpuid_info()
{
  static const char *intel_leaf2_data[256] =
  {
    [0x00] = "Null descriptor, this byte contains no information",
    [0x01] = "Instruction TLB: 4KiB pages, 4-way set associative, 32 entries",
    [0x02] = "Instruction TLB: 4MiB pages, fully associative, 2 entries",
    [0x03] = "Data TLB: 4KiB pages, 4-way set associative, 64 entries",
    [0x04] = "Data TLB: 4MiB pages, 4-way set associative, 8 entries",
    [0x05] = "Data TLB1: 4MiB pages, 4-way set associative, 32 entries",
    [0x06] = "1st-level instruction cache: 8KiBs, 4-way set associative, 32 byte line size",
    [0x08] = "1st-level instruction cache: 16KiBs, 4-way set associative, 32 byte line size",
    [0x09] = "1st-level instruction cache: 32KiBs, 4-way set associative, 64 byte line size",
    [0x0A] = "1st-level data cache: 8KiBs, 2-way set associative, 32 byte line size",
    [0x0B] = "Instruction TLB: 4MiB pages, 4-way set associative, 4 entries",
    [0x0C] = "1st-level data cache: 16KiBs, 4-way set associative, 32 byte line size",
    [0x0D] = "1st-level data cache: 16KiBs, 4-way set associative, 64 byte line size",
    [0x0E] = "1st-level data cache: 24KiBs, 6-way set associative, 64 byte line size",
    [0x1D] = "2nd-level cache: 128KiBs, 2-way set associative, 64 byte line size",
    [0x21] = "2nd-level cache: 256KiBs, 8-way set associative, 64 byte line size",
    [0x22] = "3rd-level cache: 512KiBs, 4-way set associative, 64 byte line size, 2 lines per sector",
    [0x23] = "3rd-level cache: 1MiBs, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x24] = "2nd-level cache: 1MiBs, 16-way set associative, 64 byte line size",
    [0x25] = "3rd-level cache: 2MiBs, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x29] = "3rd-level cache: 4MiBs, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x2C] = "1st-level data cache: 32KiBs, 8-way set associative, 64 byte line size",
    [0x30] = "1st-level instruction cache: 32KiBs, 8-way set associative, 64 byte line size",
    [0x40] = "No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache",
    [0x41] = "2nd-level cache: 128KiBs, 4-way set associative, 32 byte line size",
    [0x42] = "2nd-level cache: 256KiBs, 4-way set associative, 32 byte line size",
    [0x43] = "2nd-level cache: 512KiBs, 4-way set associative, 32 byte line size",
    [0x44] = "2nd-level cache: 1MiB, 4-way set associative, 32 byte line size",
    [0x45] = "2nd-level cache: 2MiB, 4-way set associative, 32 byte line size",
    [0x46] = "3rd-level cache: 4MiB, 4-way set associative, 64 byte line size",
    [0x47] = "3rd-level cache: 8MiB, 8-way set associative, 64 byte line size",
    [0x48] = "2nd-level cache: 3MiB, 12-way set associative, 64 byte line size",
    [0x49] = "3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H); 2nd-level cache: 4MiB, 16-way set associative, 64 byte line size",
    [0x4A] = "3rd-level cache: 6MiB, 12-way set associative, 64 byte line size",
    [0x4B] = "3rd-level cache: 8MiB, 16-way set associative, 64 byte line size",
    [0x4C] = "3rd-level cache: 12MiB, 12-way set associative, 64 byte line size",
    [0x4D] = "3rd-level cache: 16MiB, 16-way set associative, 64 byte line size",
    [0x4E] = "2nd-level cache: 6MiB, 24-way set associative, 64 byte line size",
    [0x4F] = "Instruction TLB: 4KiB pages, 32 entries",
    [0x50] = "Instruction TLB: 4KiB and 2-MiB or 4-MiB pages, 64 entries",
    [0x51] = "Instruction TLB: 4KiB and 2-MiB or 4-MiB pages, 128 entries",
    [0x52] = "Instruction TLB: 4KiB and 2-MiB or 4-MiB pages, 256 entries",
    [0x55] = "Instruction TLB: 2MiB or 4MiB pages, fully associative, 7 entries",
    [0x56] = "Data TLB0: 4MiB pages, 4-way set associative, 16 entries",
    [0x57] = "Data TLB0: 4KiB pages, 4-way associative, 16 entries",
    [0x59] = "Data TLB0: 4KiB pages, fully associative, 16 entries",
    [0x5A] = "Data TLB0: 2MiB or 4MiB pages, 4-way set associative, 32 entries",
    [0x5B] = "Data TLB: 4KiB and 4MiB pages, 64 entries",
    [0x5C] = "Data TLB: 4KiB and 4MiB pages,128 entries",
    [0x5D] = "Data TLB: 4KiB and 4MiB pages,256 entries",
    [0x60] = "1st-level data cache: 16KiB, 8-way set associative, 64 byte line size",
    [0x61] = "Instruction TLB: 4KiB pages, fully associative, 48 entries",
    [0x63] = "Data TLB: 2MiB or 4MiB pages, 4-way set associative, 32 entries and a separate array with 1GiB pages, 4-way set associative, 4 entries",
    [0x64] = "Data TLB: 4KiB pages, 4-way set associative, 512 entries",
    [0x66] = "1st-level data cache: 8KiB, 4-way set associative, 64 byte line size",
    [0x67] = "1st-level data cache: 16KiB, 4-way set associative, 64 byte line size",
    [0x68] = "1st-level data cache: 32KiB, 4-way set associative, 64 byte line size",
    [0x6A] = "uTLB: 4KiB pages, 8-way set associative, 64 entries",
    [0x6B] = "DTLB: 4KiB pages, 8-way set associative, 256 entries",
    [0x6C] = "DTLB: 2MiB/4MiB pages, 8-way set associative, 128 entries",
    [0x6D] = "DTLB: 1GiB pages, fully associative, 16 entries",
    [0x70] = "Trace cache: 12 K-μop, 8-way set associative",
    [0x71] = "Trace cache: 16 K-μop, 8-way set associative",
    [0x72] = "Trace cache: 32 K-μop, 8-way set associative",
    [0x76] = "Instruction TLB: 2MiB/4MiB pages, fully associative, 8 entries",
    [0x78] = "2nd-level cache: 1MiB, 4-way set associative, 64byte line size",
    [0x79] = "2nd-level cache: 128KiB, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x7A] = "2nd-level cache: 256KiB, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x7B] = "2nd-level cache: 512KiB, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x7C] = "2nd-level cache: 1MiB, 8-way set associative, 64 byte line size, 2 lines per sector",
    [0x7D] = "2nd-level cache: 2MiB, 8-way set associative, 64byte line size",
    [0x7F] = "2nd-level cache: 512KiB, 2-way set associative, 64-byte line size",
    [0x80] = "2nd-level cache: 512KiB, 8-way set associative, 64-byte line size",
    [0x82] = "2nd-level cache: 256KiB, 8-way set associative, 32 byte line size",
    [0x83] = "2nd-level cache: 512KiB, 8-way set associative, 32 byte line size",
    [0x84] = "2nd-level cache: 1MiB, 8-way set associative, 32 byte line size",
    [0x85] = "2nd-level cache: 2MiB, 8-way set associative, 32 byte line size",
    [0x86] = "2nd-level cache: 512KiB, 4-way set associative, 64 byte line size",
    [0x87] = "2nd-level cache: 1MiB, 8-way set associative, 64 byte line size",
    [0xA0] = "DTLB: 4k pages, fully associative, 32 entries",
    [0xB0] = "Instruction TLB: 4KiB pages, 4-way set associative, 128 entries",
    [0xB1] = "Instruction TLB: 2MiB pages, 4-way, 8 entries or 4MiB pages, 4-way, 4 entries",
    [0xB2] = "Instruction TLB: 4KiB pages, 4-way set associative, 64 entries",
    [0xB3] = "Data TLB: 4KiB pages, 4-way set associative, 128 entries",
    [0xB4] = "Data TLB1: 4KiB pages, 4-way associative, 256 entries",
    [0xB5] = "Instruction TLB: 4KiB pages, 8-way set associative, 64 entries",
    [0xB6] = "Instruction TLB: 4KiB pages, 8-way set associative, 128 entries",
    [0xBA] = "Data TLB1: 4KiB pages, 4-way associative, 64 entries",
    [0xC0] = "Data TLB: 4KiB and 4MiB pages, 4-way associative, 8 entries",
    [0xC1] = "Shared 2nd-Level TLB: 4KiB/2MiB pages, 8-way associative, 1024 entries",
    [0xC2] = "DTLB: 4KiB/2MiB pages, 4-way associative, 16 entries",
    [0xC3] = "Shared 2nd-Level TLB: 4KiB/2MiB pages, 6-way associative, 1536 entries. Also 1 GBbyte pages, 4-way, 16 entries",
    [0xC4] = "DTLB: 2MiB/4MiB pages, 4-way associative, 32 entries",
    [0xCA] = "Shared 2nd-Level TLB: 4KiB pages, 4-way associative, 512 entries",
    [0xD0] = "3rd-level cache: 512KiB, 4-way set associative, 64 byte line size",
    [0xD1] = "3rd-level cache: 1MiB, 4-way set associative, 64 byte line size",
    [0xD2] = "3rd-level cache: 2MiB, 4-way set associative, 64 byte line size",
    [0xD6] = "3rd-level cache: 1MiB, 8-way set associative, 64 byte line size",
    [0xD7] = "3rd-level cache: 2MiB, 8-way set associative, 64 byte line size",
    [0xD8] = "3rd-level cache: 4MiB, 8-way set associative, 64 byte line size",
    [0xDC] = "3rd-level cache: 1.5MiB, 12-way set associative, 64 byte line size",
    [0xDD] = "3rd-level cache: 3MiB, 12-way set associative, 64 byte line size",
    [0xDE] = "3rd-level cache: 6MiB, 12-way set associative, 64 byte line size",
    [0xE2] = "3rd-level cache: 2MiB, 16-way set associative, 64 byte line size",
    [0xE3] = "3rd-level cache: 4MiB, 16-way set associative, 64 byte line size",
    [0xE4] = "3rd-level cache: 8MiB, 16-way set associative, 64 byte line size",
    [0xEA] = "3rd-level cache: 12MiB, 24-way set associative, 64 byte line size",
    [0xEB] = "3rd-level cache: 18MiB, 24-way set associative, 64 byte line size",
    [0xEC] = "3rd-level cache: 24MiB, 24-way set associative, 64 byte line size",
    [0xF0] = "64-Byte prefetching",
    [0xF1] = "128-Byte prefetching",
    [0xFE] = "CPUID leaf 2 does not report TLB descriptor information; use CPUID leaf 18H to query TLB and other address translation parameters",
    [0xFF] = "CPUID leaf 2 does not report cache descriptor information, use CPUID leaf 4 to query cache parameters",
  };
  static const char *amd_leaf_x6_data[16] =
  {
    [0x0] = "L2/L3 cache or TLB is disabled",
    [0x1] = "Direct mapped",
    [0x2] = "2-way associative",
    [0x3] = "3-way associative",
    [0x4] = "4 to 5-way associative",
    [0x5] = "6 to 7-way associative",
    [0x6] = "8 to 15-way associative",
    [0x7] = "Permanently reserved",
    [0x8] = "16 to 31-way associative",
    [0x9] = "Value for all fields should be determined from Fn8000_001D",
    [0xA] = "32 to 47-way associative",
    [0xB] = "48 to 63-way associative",
    [0xC] = "64 to 95-way associative",
    [0xD] = "96 to 127-way associative",
    [0xE] = "More than 128-way associative but not fully associative",
    [0xF] = "Fully associative",
  };
  unsigned int max_eax,max_eeax,idx,max_idx;
  union
  { // little-endian shenanigans
    unsigned char c[16];
    unsigned int i[4];
  }
  u;
  cpuid_data_t data;
  int is_intel,is_amd;

  // leaf 0x00
  cpuid(0x00u,0u,&data);
  max_eax = data.eax;
  u.i[0] = data.ebx;
  u.i[1] = data.edx;
  u.i[2] = data.ecx;
  u.i[3] = 0u;
  printf("leaf 0x00 data:\n");
  printf("  [%s] %08X %08X %08X %08X\n",u.c,data.eax,data.ebx,data.ecx,data.edx);
  is_intel = (data.ebx == 0x756E6547u && data.ecx == 0x6C65746Eu && data.edx == 0x49656E69u) ? 1 : 0;
  is_amd = (data.ebx == 0x68747541u && data.ecx == 0x444D4163u && data.edx == 0x69746E65u) ? 1 : 0;
  // leaf 0x80000000
  cpuid(0x80000000u,0u,&data);
  max_eeax = data.eax;
  // intel leaf 0x02 (cache and TLB information)
  if(is_intel != 0 && max_eax >= 0x02u)
  {
    cpuid(0x02u,0u,&data);
    printf("leaf 0x02 data:\n");
    //printf("  [%08X%08X%08X%08X]\n",data.eax,data.ebx,data.ecx,data.edx);
    u.i[0] = ((data.eax & 0x80000000u) == 0u) ? data.eax : 0x00000000u;
    u.i[1] = ((data.ebx & 0x80000000u) == 0u) ? data.ebx : 0x00000000u;
    u.i[2] = ((data.ecx & 0x80000000u) == 0u) ? data.ecx : 0x00000000u;
    u.i[3] = ((data.edx & 0x80000000u) == 0u) ? data.edx : 0x00000000u;
    if(u.c[0] != 0x01)
      printf("  [warning: eax[7:0] != 0x01]\n");
    for(idx = 1u;idx < 16u;idx++)
    {
      if(u.c[idx] != 0u && intel_leaf2_data[u.c[idx]] != NULL)
        printf("  [%s]\n",intel_leaf2_data[u.c[idx]]);
      if(u.c[idx] != 0u && intel_leaf2_data[u.c[idx]] == NULL)
        printf("  [UNKNOWN]\n");
    }
  }
  // intel leaf 0x04 data (deterministic cache parameters)
  if(is_intel != 0 && max_eax >= 0x04u)
    for(idx = 0u;;idx++)
    {
      cpuid(0x04u,idx,&data);
      if((data.eax & 0x1Fu) == 0u)
        break;
      if(idx == 0u)
        printf("leaf 0x04 data:\n");
      switch(data.eax & 0x1Fu)
      {
        case 0x01u: printf("  [data cache]");        break;
        case 0x02u: printf("  [instruction cache]"); break;
        case 0x03u: printf("  [unified cache]");     break;
        default:    printf("  [RESERVED]");          break;
      }
      printf(" [level %u]",(data.eax >> 5) & 0x7u);
      printf(" [%u-way]",1u + ((data.ebx >> 22) & 0x3FFu));
      printf(" [%u line size]",1u + (data.ebx & 0xFFFu));
      printf(" [%sinclusive cache]",((data.edx & 0x1u) == 0u) ? "non " : "");
      printf(" [%s index function]",((data.edx & 0x2u) == 0u) ? "linear" : "complex");
      printf(" [%u KiB]",((1u + ((data.ebx >> 22) & 0x3FFu)) * (1u + ((data.ebx >> 12) & 0x3FFu)) * (1u + (data.ebx & 0xFFFu)) * (1u + data.ecx)) >> 10);
      printf("\n");
    }
  // intel leaf 0x18 data (deterministic cache parameters)
  if(is_intel != 0 && max_eax >= 0x18u)
  {
    printf("leaf 0x18 data:\n");
    max_idx = 0u;
    for(idx = 0u;idx <= max_idx;idx++)
    {
      cpuid(0x18u,idx,&data);
      if(idx == 0u)
        max_idx = data.eax;
      switch((data.edx) & 0x1Fu)
      {
        case 0x00u: printf("  [Null]");            break;
        case 0x01u: printf("  [data TLB]");        break;
        case 0x02u: printf("  [instruction TLB]"); break;
        case 0x03u: printf("  [unified TLB]");     break;
        case 0x04u: printf("  [load only TLB]");   break;
        case 0x05u: printf("  [store only TLB]");  break;
        default:    printf("  [RESERVED]");        break;
      }
      printf(" [level %u]",(data.edx >> 5) & 0x7u);
      if((data.ebx & 0x1) != 0u) printf(" [for 4KiB pages]");
      if((data.ebx & 0x2) != 0u) printf(" [for 2MiB pages]");
      if((data.ebx & 0x4) != 0u) printf(" [for 4MiB pages]");
      if((data.ebx & 0x8) != 0u) printf(" [for 1GiB pages]");
      printf(" [%u-way]",(data.ebx >> 16) & 0xFFFFu);
      printf(" [%u sets]",data.ecx);
      printf("\n");
    }
  }
  // amd leaf 0x80000005 data (L1 cache and TLB)
  if(is_amd != 0 && max_eeax >= 0x80000005u)
  {
    printf("leaf 0x80000005 data:\n");
    cpuid(0x80000005u,idx,&data);
    printf("  [instruction cache] [level 1] [%u-way] [%u line size] [%u KiB]\n",(data.edx >> 16) & 0xFFu,data.edx & 0xFFu,(data.edx >> 24) & 0xFFu);
    printf("  [data cache] [level 1] [%u-way] [%u line size] [%u KiB]\n",(data.ecx >> 16) & 0xFFu,data.ecx & 0xFFu,(data.ecx >> 24) & 0xFFu);
    printf("  [instruction TLB] [level 1] [for 4KiB pages] [%u-way] [%u entries]\n",(data.ebx >> 8) & 0xFFu,data.ebx & 0xFFu);
    printf("  [data TLB] [level 1] [for 4KiB pages] [%u-way] [%u entries]\n",(data.ebx >> 24) & 0xFFu,(data.ebx >> 16) & 0xFFu);
    printf("  [instruction TLB] [level 1] [for 2MiB/4MiB pages] [%u-way] [%u entries (for 2MiB pages)]\n",(data.eax >> 8) & 0xFFu,data.eax & 0xFFu);
    printf("  [data TLB] [level 1] [for 2MiB/4MiB pages] [%u-way] [%u entries (for 2MiB pages)]\n",(data.eax >> 24) & 0xFFu,(data.eax >> 16) & 0xFFu);
  }
  // amd leaf 0x80000006 data (L2 cache and TLB and L3 cache)
  if(is_amd != 0 && max_eeax >= 0x80000006u)
  {
    printf("leaf 0x80000006 data:\n");
    cpuid(0x80000006u,idx,&data);
    printf("  [unified cache] [level 2] [%s] [%u line size] [%u KiB]\n",amd_leaf_x6_data[(data.ecx >> 12) & 0xFu],data.ecx & 0xFFu,(data.ecx >> 16) & 0xFFFFu);
    printf("  [unified cache] [level 3] [%s] [%u line size] [%u KiB]\n",amd_leaf_x6_data[(data.edx >> 12) & 0xFu],data.edx & 0xFFu,512u * ((data.edx >> 18) & 0x3FFFu));
    printf("  [instruction TLB] [level 2] [for 4KiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.ebx >> 12) & 0xFu],data.ebx & 0xFFFu);
    printf("  [data TLB] [level 2] [for 4KiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.ebx >> 28) & 0xFu],(data.ebx >> 16) & 0xFFFu);
    printf("  [instruction TLB] [level 2] [for 2MiB/4MiB pages] [%s] [%u entries (for 2MiB pages)]\n",amd_leaf_x6_data[(data.eax >> 12) & 0xFu],data.eax & 0xFFFu);
    printf("  [data TLB] [level 2] [for 2MiB/4MiB pages] [%s] [%u entries (for 2MiB pages)]\n",amd_leaf_x6_data[(data.eax >> 28) & 0xFu],(data.eax >> 16) & 0xFFFu);
  }
  // amd leaf 0x80000019 data (TLB)
  if(is_amd != 0 && max_eeax >= 0x80000019u)
  {
    printf("leaf 0x80000019 data:\n");
    cpuid(0x80000019u,idx,&data);
    printf("  [instruction TLB] [level 1] [for 1GiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.eax >> 12) & 0xFu],data.eax & 0xFFFu);
    printf("  [data TLB] [level 1] [for 1GiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.ebx >> 28) & 0xFu],(data.ebx >> 16) & 0xFFFu);
    printf("  [instruction TLB] [level 2] [for 1GiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.ebx >> 12) & 0xFu],data.ebx & 0xFFFu);
    printf("  [data TLB] [level 2] [for 1GiB pages] [%s] [%u entries]\n",amd_leaf_x6_data[(data.ebx >> 28) & 0xFu],(data.ebx >> 16) & 0xFFFu);
  }
  // amd leaf 0x8000001D data (cache properties)
  if(is_amd != 0 && max_eeax >= 0x8000001Du)
    for(idx = 0u;;idx++)
    {
      cpuid(0x8000001Du,idx,&data);
      if((data.eax & 0x1Fu) == 0u)
        break;
      if(idx == 0u)
        printf("leaf 0x8000001D data:\n");
      switch(data.eax & 0x1Fu)
      {
        case 0x01u: printf("  [data cache]");        break;
        case 0x02u: printf("  [instruction cache]"); break;
        case 0x03u: printf("  [unified cache]");     break;
        default:    printf("  [RESERVED]");          break;
      }
      printf(" [level %u]",(data.eax >> 5) & 0x7u);
      printf(" [%u-way]",1u + ((data.ebx >> 22) & 0x3FFu));
      printf(" [%u line size]",1u + (data.ebx & 0xFFFu));
      printf(" [%sinclusive cache]",((data.edx & 0x1u) == 0u) ? "non " : "");
      printf(" [%u KiB]",((1u + ((data.ebx >> 22) & 0x3FFu)) * (1u + ((data.ebx >> 12) & 0x3FFu)) * (1u + (data.ebx & 0xFFFu)) * (1u + data.ecx)) >> 10);
      printf("\n");
    }
}

#endif


//
// memory allocation and deallocation
//
// linux kernel hugepages info:
//   https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
// to see general memory information:
//   cat /proc/meminfo
// to see data about 2MiB huge pages:
//   ls -al /sys/kernel/mm/hugepages/hugepages-2048kB
// to see data about 1GiB huge pages:
//   ls -al /sys/kernel/mm/hugepages/hugepages-1048576kB
// to reserve 512 2MiB huge pages:
//   sudo echo 512 >/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
// to reserve 1 1GiB huge pages:
//   sudo echo 1 >/sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages
//
// use the command
//   cpuid -1 -l 2
// to get information about the TLB caches
//

static void *memory;
static int memory_size;
static int page_size;

static void alloc_memory(int try_to_use_huge_pages)
{
  memory = MAP_FAILED;
  memory_size = 1 << 30;
  page_size = 0;
#ifdef MAP_HUGETLB
  // try to allocate 1 1GiB page
  if(try_to_use_huge_pages > 1 && memory == MAP_FAILED)
  {
    memory = mmap(NULL,(size_t)memory_size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_1GB,-1,(off_t)0);
    if(memory != MAP_FAILED)
      page_size = 1 << 30;
  }
  // if that fails, try to allocate 512 2MiB page
  if(try_to_use_huge_pages > 0 && memory == MAP_FAILED)
  {
    memory = mmap(NULL,(size_t)memory_size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB,-1,(off_t)0);
    if(memory != MAP_FAILED)
      page_size = 2 << 20;
  }
#endif
  // last resort, try a normal mmap (malloc does this so we do it directly)
  if(memory == MAP_FAILED)
  {
    memory = mmap(NULL,(size_t)memory_size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,(off_t)0);
    if(memory != MAP_FAILED)
      page_size = getpagesize();
  }
  // terminate on failure
  if(memory == MAP_FAILED)
  {
    fprintf(stderr,"unable to allocate memory\n");
    exit(1);
  }
  // advise
#ifdef MAP_HUGETLB
  if(try_to_use_huge_pages > 1 && page_size < (1 << 30))
    fprintf(stderr,"page size too small: as root, try \"echo 1 >/sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages\" to reserve enough huge 1GiB pages\n");
  else if(try_to_use_huge_pages > 0 && page_size < (2 << 20))
    fprintf(stderr,"page size too small: as root, try \"echo 512 >/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages\" to reserve enough huge 2MiB pages\n");
#endif
}

static void free_memory(void)
{
  if(memory != MAP_FAILED)
    munmap(memory,memory_size);
  memory = MAP_FAILED;
  memory_size = 0;
  page_size = 0;
}


//
// generate a pseudo-random sequence with period m and going over all integers 0 <= k < m
//
// theorem A (3.2.1.2 of TAOCP): x(n+1)=a*x(n)+c mod m has period m if and only if
//   i) gcd(c,m) = 1
//  ii) b = a-1 is a multiple of p for every prime dividing m
// iii) b is a multiple of 4 if m is a multiple of 4
//
// see https://en.wikipedia.org/wiki/Low-discrepancy_sequence for an explanation of our use of the constant 0.618...
//

static int m,a,c;

static void set_generator_parameters(void)
{ // input is m, output is a,c
  int t,d,p;

  if(m < 100 || m > 1000000000)
  {
    fprintf(stderr,"bad m value (m=%d)\n",m);
    exit(1);;
  }
  // factor m
  t = m;
  p = 1;
  for(d = 2;d * d <= t;d = (d + 1) | 1)
    if(t % d == 0)
    {
      p *= d;
      do
        t /= d;
      while(t % d == 0);
    }
  if(t > 1)
    p *= t;
  if(m % 4 == 0)
    p *= 2;
  // choose a
  a = p * (int)floor(0.61803398874989484820 * (double)m / (double)p) + 1;
  if(a == 1)
  {
    fprintf(stderr,"bad a value (m=%d,p=%d,a=%d)\n",m,p,a);
    exit(1);;
  }
  c = 1;
  // test
  t = d = 0;
  do
  {
    t = (int)(((unsigned long)a * (unsigned long)t + (unsigned long)c) % (unsigned long)m);
    d++;
  }
  while(t != 0);
  if(d != m)
  {
    fprintf(stderr,"m=%d a=%d c=%d is bad\n",m,a,c);
    exit(1);
  }
}


//
// measure read and write bandwidth for 8-byte accesses (in GiB/s)
//
// because we do not use non-temporal hint read and writes, the actual
// read and write bandwidth measurements may depend on how many reads
// and writes the L1 data cache can handle in each clock cycle
//

static double read_bandwidth;
static double write_bandwidth;

static void measure_bandwidth(int size)
{
  unsigned long t0,t1,t2,t3,n_ops,*p;
  int i,n,n_passes;
  double dt;

  // we will work with 64-bit reads and writes
  size /= 8;
  // make sure size is a multiple of 4
  size -= size % 4;
  // decide how many passes will be done
  n_ops = 1ul << 30; // desired number of read/write operations
  n_passes = (int)(n_ops / (unsigned long)size);
  if(n_passes < 10)
    n_passes = 10;
  n_ops = (unsigned long)n_passes * (unsigned long)size;
  // initialize memory
  p = (unsigned long *)memory;
  for(i = 0;i < size;i++)
    p[i] = (unsigned long)i;
  // do the reads
  dt = cpu_time();
  t0 = t1 = t2 = t3 = 0ul;
  for(n = 0;n < n_passes;n++)
    for(i = 0,p = (unsigned long *)memory;i < size;i += 4,p += 4)
    { // 4-way loop unrolling
      t0 += p[0];
      t1 += p[1];
      t2 += p[2];
      t3 += p[3];
    }
  dt = cpu_time() - dt;
  read_bandwidth = 8.0 * (double)n_ops / dt / (double)(1 << 30);
  // make sure the reads were actually performed
  if(t0 + t1 + t2 + t3 == 0ul)
    printf("unexpected zero sum!\n");
  // do the writes
  dt = cpu_time();
  t0 = 1ul;
  t1 = 2ul;
  t2 = 3ul;
  t3 = 4ul;
  for(n = 0;n < n_passes;n++)
  {
    for(i = 0,p = (unsigned long *)memory;i < size;i += 4,p += 4)
    { // 4-way loop unrolling
      p[0] = t0;
      p[1] = t1;
      p[2] = t2;
      p[3] = t3;
    }
    t0++;
    t1++;
    t2++;
    t3++;
  }
  dt = cpu_time() - dt;
  write_bandwidth = 8.0 * (double)n_ops / dt / (double)(1 << 30);
}


//
// measure latency for 1, 2, and 3 random linked-list walks (in ns)
//

#ifndef PRE_PADDING
# define PRE_PADDING  0
#endif
#ifndef POS_PADDING
# define POS_PADDING  56
#endif
#if PRE_PADDING + POS_PADDING != 56
# error "bad PRE_PADDING+POS_PADDING"
#endif

typedef struct block_s
{
  char pre_padding[PRE_PADDING];
  struct block_s *next;
  char pos_padding[POS_PADDING];
}
block_t __attribute__ ((aligned(64)));

static double latency1,latency2,latency3;

static void measure_latency(int size)
{
  int i,n,n_passes,x0,x1;
  unsigned long n_hops;
  block_t *b0,*b1,*b2;
  double dt;

  // pseudo-random generator parameters
  m = size / 64;
  set_generator_parameters();
  // circular linked list initialization
  b0 = (block_t *)memory;
  x0 = 0;
  for(i = 0;i < m;i++)
  {
    x1 = (int)(((unsigned long)a * (unsigned long)x0 + (unsigned long)c) % (unsigned long)m);
    b0[x0].next = &b0[x1];
    x0 = x1;
  }
  // test
  b0 = (block_t *)memory;
  for(i = 0;i < m;i++)
  {
    if(i > 0 && b0 == (block_t *)memory)
      break;
    b0 = b0->next;
  }
  if(i != m || b0 != (block_t *)memory)
  {
    fprintf(stderr,"bad circular linked list (i=%d)\n",i);
    exit(1);
  }
  // decide how many passes will be done
  n_hops = 1ul << 28; // desired number of hops (jumps)
  n_passes = 6 * (int)(n_hops / (unsigned long)(6 * m));
  if(n_passes < 12)
    n_passes = 12;
  n_hops = (unsigned long)n_passes * (unsigned long)m;
  // measure (1 walk)
  dt = cpu_time();
  b0 = (block_t *)memory;
  for(n = 0;n < n_passes;n++)
    for(i = 0;i < m;i++)
      b0 = b0->next;
  dt = cpu_time() - dt;
  latency1 = 1e9 * dt / (double)n_hops;
  if(b0 != (block_t *)memory)
  { // make sure the reads were actually performed
    fprintf(stderr,"unexpected b0 value\n");
    exit(1);
  }
  // measure (2 walks)
  dt = cpu_time();
  b0 = (block_t *)memory;
  b1 = (block_t *)memory + (m / 2);
  for(n = 0;n < n_passes;n += 2)
    for(i = 0;i < m;i++)
    {
      b0 = b0->next;
      b1 = b1->next;
    }
  dt = cpu_time() - dt;
  latency2 = 1e9 * dt / (double)n_hops;
  if(b0 != (block_t *)memory || b1 != (block_t *)memory + (m / 2))
  { // make sure the reads were actually performed
    fprintf(stderr,"unexpected b0 or b1 value\n");
    exit(1);
  }
  // measure (3 walks)
  dt = cpu_time();
  b0 = (block_t *)memory;
  b1 = (block_t *)memory + (m / 3);
  b2 = (block_t *)memory + ((2 * m) / 3);
  for(n = 0;n < n_passes;n += 3)
    for(i = 0;i < m;i++)
    {
      b0 = b0->next;
      b1 = b1->next;
      b2 = b2->next;
    }
  dt = cpu_time() - dt;
  latency3 = 1e9 * dt / (double)n_hops;
  if(b0 != (block_t *)memory || b1 != (block_t *)memory + (m / 3) || b2 != (block_t *)memory + ((2 * m) / 3))
  { // make sure the reads were actually performed
    fprintf(stderr,"unexpected b0, b1 or b2 value\n");
    exit(1);
  }
}


//
// main program
//

int main(void)
{
  int i,j,size,p;

#ifdef __x86_64__
  cpuid_info();
#endif
  for(p = 0;p <= 2;p++)
  {
#ifndef MAP_HUGETLB
    if(p > 0)
      continue;
#endif
    alloc_memory(p);
    if(p == 2 && page_size != (1 << 30))
      goto skip;
    if(p == 1 && page_size != (2 << 20))
      goto skip;
    fprintf(stderr,"measuring with page_size = %d\n",page_size);
    printf("\n\n# page_size=%d\n",page_size);
    i = 16;
    j = 9;
    printf("#    bytes   GiB/s   GiB/s       ns       ns       ns\n");
    printf("#--------- ------- ------- -------- -------- --------\n");
    printf("#     size    read   write latency1 latency2 latency3\n");
    printf("#--------- ------- ------- -------- -------- --------\n");
    for(;;)
    {
      // measure
      size = i << j;
      if(size > memory_size)
        break;
      measure_bandwidth(size);
      measure_latency(size);
      // report
      printf("%10d %7.3f %7.3f %8.3f %8.3f %8.3f\n",size,read_bandwidth,write_bandwidth,latency1,latency2,latency3);
      // next size
      if(i == 28)
      {
        i = 16;
        j++;
      }
      else
        i += 4;
    }
    printf("#--------- ------- ------- -------- -------- --------\n");
skip:
    free_memory();
  }
}
