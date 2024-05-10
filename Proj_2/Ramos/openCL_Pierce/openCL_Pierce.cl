//
//  Pedro Ramos (May 2024)
//
//  This program executes the Pierce Expansion algorithm for a given
// B value which is based on the current thread Idx and iteration.
//  The value is then assigned to the global buffer.
//

__kernel
//__attribute__((work_group_size_hint(128,1,1)))
void pierce(__global char *buffer, int buffer_size, unsigned int iter, unsigned int thread_count) {

  size_t idx = get_global_id(0);

  if(idx >= (size_t)0 && idx < (size_t)buffer_size) {

    //  The a, c, and temporary c variables
    unsigned int a, c, tempC;

    //  Initialize the final biggest c found
    char bigC = 0;

    //  Calculate the current b, which is the current iteration times
    // the thread count of each iteration plus this thread's index
    unsigned int b = idx + (iter * thread_count);

    //  For each value from 1 to b
    for (a = 1u; a < b; a++) {

      c = 0;
      
      //  Whole divide a until it hits 0 
      for (tempC = a; tempC != 0; c++) {
        tempC = b % tempC;
      }
      
      //  Save how many times the division was performed
      if (c > bigC) {
        bigC = c;
      }
    }
  
    //  Save the largest division count found to the buffer
    buffer[idx] = bigC;
  }
}
