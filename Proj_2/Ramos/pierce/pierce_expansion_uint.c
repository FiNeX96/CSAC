// Pedro Ramos,  February 2024
//
// Pierce Expansion example to run on a GPU
// Version 1

//  Includes
#include <stdio.h>
#include <stdlib.h>


typedef unsigned int u32_t;

//  Pierce Expansion Function
static void pierce(unsigned int * topA, unsigned int b, unsigned int * topC) {
  unsigned int a, c, tempC, bigC, bigA;

  bigA = bigC = 0;

  //  1 <= a <= b
  for (a = 1u; a < b; a++) {
    c = 0;
    for (tempC = a; tempC != 0; c++) {
      tempC = b % tempC;
    }
    
    if (c > bigC) {
      bigA = a;
      bigC = c;
    }

  }

  *topA = bigA;
  *topC = bigC;
}


//  Main Program
int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Please insert a max B parameter, none inserted\nUsage -> ./pierce_expansion <uint>\n");
    return 1;
  }
  int param = atoi(argv[1]);

  if (param < 1) {
    printf("B param must be larger than 0!\n");
    return 1;
  }

  unsigned int bigA = 0;
  unsigned int c = 0;
  unsigned int biggestC = 0;
  
  for (unsigned int b = 0; b < param; b++) {
    pierce(&bigA, b, &c);

    if (c > biggestC) {
      //printf("a: %d | b: %d | c: %d\n", bigA, b, c);

      printf("       B: %6d    |      C: %04d    |    Hex:  %04X\n", b, c, c & 0xFFFF);
      biggestC = c;
    }
  }

  return 0;
}

