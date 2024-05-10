//
// Pedro Ramos (May 2024)
//
// Simple OpenCL kernel (each thread writes one char)
//

__kernel
//__attribute__((work_group_size_hint(128,1,1)))
void pierce(__global int *buffer,int buffer_size, int iter)
{
  size_t idx;

  idx = get_global_id(0); // our kernel only has one dimension, so this is enough to identify the thread
  if(idx >= (size_t)0 && idx < (size_t)buffer_size)
    buffer[idx] = idx * (iter + 1);
}
