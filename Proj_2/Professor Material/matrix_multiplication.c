//
// Tomás Oliveira e Silva,   February 2024
//
// measure the time it takes to multiply two NxN matrices using three different matrix data layouts
//

#include <stdio.h>
#include <stdlib.h>
#include "elapsed_time.h"


//
// the largest matrix size (use a power of two to avoid complications allocating memory)
//

#ifndef N_LOG2
# define N_LOG2  10
#endif
#define N  (1 << N_LOG2)


//
// the memory layout to use
//   1 --- row major (C)
//   2 --- column major (fortran)
//   3 --- Morton layout (custom)
//
// m[i3i2i1i0][j3j2j1j0]
//   i3i2i1i0 -> addr_i
//   j3j2j1j0 -> addr_j
// m[i3i2i1i0][j3j2j1j0] -> flat_m[addr_i + addr_j] or flat_m[addr_i | addr_j]
//

#ifndef LAYOUT
# define LAYOUT  1
#endif


//
// row major layout --- m[i3i2i1i0][j3j2j1j0] -> addr is i3i2i1i0j3j2j1j0
//

#if LAYOUT == 1

# define NEXT_ADDR_I(addr_i,n)  ((addr_i) + (n))
# define NEXT_ADDR_J(addr_j,n)  ((addr_j) + 1)

# define MAX_ADDR_I(n)          ((n) * (n))
# define MAX_ADDR_J(n)          (n)

#endif


//
// column major layout --- m[i3i2i1i0][j3j2j1j0] -> addr is j3j2j1j0i3i2i1i0
//

#if LAYOUT == 2

# define NEXT_ADDR_I(addr_i,n)  ((addr_i) + 1)
# define NEXT_ADDR_J(addr_j,n)  ((addr_j) + (n))

# define MAX_ADDR_I(n)          (n)
# define MAX_ADDR_J(n)          ((n) * (n))

#endif


//
// Morton layout m[i3i2i1i0][j3j2j1j0] -> addr is i3j3i2j2i1j1i0j0
//

#if LAYOUT == 3

# define MASK_ADDR_I            0xAAAAAAAA  // 0b10101010...
# define MASK_ADDR_J            0x55555555  // 0b01010101...

# define NEXT_ADDR_I(addr_i,n)  (((addr_i) + MASK_ADDR_J + 1) & MASK_ADDR_I)
# define NEXT_ADDR_J(addr_j,n)  (((addr_j) + MASK_ADDR_I + 1) & MASK_ADDR_J)

# define MAX_ADDR_I(n)          (to_morton_1d(n,1))
# define MAX_ADDR_J(n)          (to_morton_1d(n,0))

static int to_morton_1d(int x,int s0)
{ // abcd -> a0b0c0d
  int r,s;

  for(r = 0,s = s0;x != 0;x >>= 1,s += 2)
    r += (x & 1) << s;
  return r;
}

#endif


//
// the matrix multiplication functions --- c[i][j] = sum_k a[i][k] * b[k][j]
//
// warning --- the c matrix is expected to have already been initialized with zeros
//

#define MAX_I  int max_i = MAX_ADDR_I(n)
#define MAX_J  int max_j = MAX_ADDR_J(n)
#define MAX_K  int max_ka = MAX_ADDR_J(n)

#define FOR_I  for(int addr_i = 0;addr_i < max_i;addr_i = NEXT_ADDR_I(addr_i,n))
#define FOR_J  for(int addr_j = 0;addr_j < max_j;addr_j = NEXT_ADDR_J(addr_j,n))
#define FOR_K  for(int addr_ka = 0,addr_kb = 0;addr_ka < max_ka;addr_ka = NEXT_ADDR_J(addr_ka,n),addr_kb = NEXT_ADDR_I(addr_kb,n))

#define make_function(var1,var2,var3)                                                                                                           \
static void matrix_multiplication_ ## var1 ## var2 ## var3(const int n,const double * restrict a,const double * restrict b,double * restrict c) \
{                                                                                                                                               \
  MAX_I; MAX_J; MAX_K;                                                                                                                          \
  FOR_ ## var1                                                                                                                                  \
    FOR_ ## var2                                                                                                                                \
      FOR_ ## var3                                                                                                                              \
        c[addr_i + addr_j] += a[addr_i + addr_ka] * b[addr_kb + addr_j];                                                                        \
}

make_function(I,J,K)
make_function(I,K,J)
make_function(J,I,K)
make_function(J,K,I)
make_function(K,I,J)
make_function(K,J,I)


//
// the matrix multiplication test function
//

typedef void function_t(const int n,const double * restrict a,const double * restrict b,double * restrict c);

int main(void)
{
  double *a,*b,*c,dt,t_sum,t_min,t_max,t[6];
  function_t *f;
  int i,j,n;

  a = (double *)malloc((size_t)N * (size_t)N * sizeof(double));
  b = (double *)malloc((size_t)N * (size_t)N * sizeof(double));
  c = (double *)malloc((size_t)N * (size_t)N * sizeof(double));
  if(a == NULL || b == NULL || c == NULL)
  {
    fprintf(stderr,"out of memory\n");
    return 1;
  }
  for(i = 0;i < N * N;i++)
    a[i] = b[i] = c[i] = 1.0;
  // for all n (but avoid very small values of n)
  printf("#  n     IJK     IKJ     JIK     JKI     KIJ     KJI\n");
  printf("#--- ------- ------- ------- ------- ------- -------\n");
  for(n = 16;n <= N;n++)
  {
    fprintf(stderr,"%d\r",n);
    // for each multiplication function
    for(i = 0;i < 6;i++)
    {
      // select function
      switch(i)
      {
        case 0: f = matrix_multiplication_IJK; break;
        case 1: f = matrix_multiplication_IKJ; break;
        case 2: f = matrix_multiplication_JIK; break;
        case 3: f = matrix_multiplication_JKI; break;
        case 4: f = matrix_multiplication_KIJ; break;
        case 5: f = matrix_multiplication_KJI; break;
      }
      // 6 runs
      t_sum = t_min = t_max = 0.0;
      for(j = 0;j < 6;j++)
      {
        dt = cpu_time();
        (*f)(n,a,b,c);
        dt = cpu_time() - dt;
        t_sum += dt;
        if(j == 0 || dt < t_min)
          t_min = dt;
        if(j == 0 || dt > t_max)
          t_max = dt;
      }
      // record execution time (ignore min and max value, mean of the rest, divide by n^3)
      t[i] = 1e9 * (t_sum - t_min - t_max) / 4.0 / (double)n / (double)n / (double)n;
    }
    // report
    printf("%4d %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f\n",n,t[0],t[1],t[2],t[3],t[4],t[5]);
  }
  printf("#--- ------- ------- ------- ------- ------- -------\n");
  // clean up and exit
  free(a);
  free(b);
  free(c);
  fprintf(stderr,"done\n");
  return 0;
}
