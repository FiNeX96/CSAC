#include <stdio.h>

typedef unsigned int u32_t;

void Pierce(u32_t b,u32_t *C,u32_t *A)
{
  u32_t a,t,c,CC,AA;

  CC = AA = 0;
  for(a = 1u;a < b;a++)
  {
    for(t = a,c = 0u;t != 0u;t = b % t,c++)
      ;
    if(c > CC)
    {
      CC = c;
      AA = a;
    }
  }
  *C = CC;
  *A = AA;
}

int main(void)
{
  u32_t b,C,A,CC;

  CC = 0u;
  for(b = 2u;b <= 100000u;b++)
  {
    Pierce(b,&C,&A);
    if(C >= CC)
    {
      printf("%u %u %u\n",b,CC = C,A);
      fflush(stdout);
    }
  }
}
