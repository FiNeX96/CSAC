//
// Tomás Oliveira e Silva,  February 2018
//
// Hello world program in OpenCL
//
// What it does:
//   the OpenCL kernel initializes an array with the string "Hello, world!"
//

#include <stdio.h>
#include <stdlib.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS  // for the clCreateCommandQueue function
#include <CL/cl.h>


//
// Macro that can be used to call an OpenCL function and to test its return value
// It can, and should, be used to test the return value of calls such as
//   e = clGetPlatformIDs(1,&platform_id[0],&num_platforms);
// In this case, f_name is clGetPlatformIDs and args is (1,&platform_id[0],&num_platforms)
//

#define cl_call(f_name,args)                                                                     \
  do {                                                                                           \
    cl_int e = f_name args;                                                                      \
    if(e != CL_SUCCESS) {                                                                        \
      /* the call failed, terminate the program */                                               \
      fprintf(stderr,"" # f_name "() returned %s (line %d)\n",cl_error_string((int)e),__LINE__); \
      exit(1);                                                                                   \
    }                                                                                            \
  }                                                                                              \
  while(0)


//
// Another macro that can be used to call an OpenCL function and to test its return value
// It can, and should, be used the test the error code value of calls such as
//   context = clCreateContext(NULL,1,&device_id[0],NULL,NULL,&e);
// In this case, f_name is context = clCreateContext and args is (NULL,1,&device_id[0],NULL,NULL,&e)
//

#define cl_call_alt(f_name,args)                                                                 \
  do {                                                                                           \
    cl_int e;                                                                                    \
    f_name args;                                                                                 \
    if(e != CL_SUCCESS) {                                                                        \
      /* the call failed, terminate the program */                                               \
      fprintf(stderr,"" # f_name "() returned %s (line %d)\n",cl_error_string((int)e),__LINE__); \
      exit(1);                                                                                   \
    }                                                                                            \
  }                                                                                              \
  while(0)


//
// "User-friendly" description of the OpenCL error codes
//

static char *cl_error_string(int e) {
  static const char *error_description[100] = {
    // warning: C99 array initialization feature
    [ 0] = "CL_SUCCESS",
    [ 1] = "CL_DEVICE_NOT_FOUND",
    [ 2] = "CL_DEVICE_NOT_AVAILABLE",
    [ 3] = "CL_COMPILER_NOT_AVAILABLE",
    [ 4] = "CL_MEM_OBJECT_ALLOCATION_FAILURE",
    [ 5] = "CL_OUT_OF_RESOURCES",
    [ 6] = "CL_OUT_OF_HOST_MEMORY",
    [ 7] = "CL_PROFILING_INFO_NOT_AVAILABLE",
    [ 8] = "CL_MEM_COPY_OVERLAP",
    [ 9] = "CL_IMAGE_FORMAT_MISMATCH",
    [10] = "CL_IMAGE_FORMAT_NOT_SUPPORTED",
    [11] = "CL_BUILD_PROGRAM_FAILURE",
    [12] = "CL_MAP_FAILURE",
    [13] = "CL_MISALIGNED_SUB_BUFFER_OFFSET",
    [14] = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST",
    [15] = "CL_COMPILE_PROGRAM_FAILURE",
    [16] = "CL_LINKER_NOT_AVAILABLE",
    [17] = "CL_LINK_PROGRAM_FAILURE",
    [18] = "CL_DEVICE_PARTITION_FAILED",
    [19] = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE",
    [30] = "CL_INVALID_VALUE",
    [31] = "CL_INVALID_DEVICE_TYPE",
    [32] = "CL_INVALID_PLATFORM",
    [33] = "CL_INVALID_DEVICE",
    [34] = "CL_INVALID_CONTEXT",
    [35] = "CL_INVALID_QUEUE_PROPERTIES",
    [36] = "CL_INVALID_COMMAND_QUEUE",
    [37] = "CL_INVALID_HOST_PTR",
    [38] = "CL_INVALID_MEM_OBJECT",
    [39] = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    [40] = "CL_INVALID_IMAGE_SIZE",
    [41] = "CL_INVALID_SAMPLER",
    [42] = "CL_INVALID_BINARY",
    [43] = "CL_INVALID_BUILD_OPTIONS",
    [44] = "CL_INVALID_PROGRAM",
    [45] = "CL_INVALID_PROGRAM_EXECUTABLE",
    [46] = "CL_INVALID_KERNEL_NAME",
    [47] = "CL_INVALID_KERNEL_DEFINITION",
    [48] = "CL_INVALID_KERNEL",
    [49] = "CL_INVALID_ARG_INDEX",
    [50] = "CL_INVALID_ARG_VALUE",
    [51] = "CL_INVALID_ARG_SIZE",
    [52] = "CL_INVALID_KERNEL_ARGS",
    [53] = "CL_INVALID_WORK_DIMENSION",
    [54] = "CL_INVALID_WORK_GROUP_SIZE",
    [55] = "CL_INVALID_WORK_ITEM_SIZE",
    [56] = "CL_INVALID_GLOBAL_OFFSET",
    [57] = "CL_INVALID_EVENT_WAIT_LIST",
    [58] = "CL_INVALID_EVENT",
    [59] = "CL_INVALID_OPERATION",
    [60] = "CL_INVALID_GL_OBJECT",
    [61] = "CL_INVALID_BUFFER_SIZE",
    [62] = "CL_INVALID_MIP_LEVEL",
    [63] = "CL_INVALID_GLOBAL_WORK_SIZE",
    [64] = "CL_INVALID_PROPERTY",
    [65] = "CL_INVALID_IMAGE_DESCRIPTOR",
    [66] = "CL_INVALID_COMPILER_OPTIONS",
    [67] = "CL_INVALID_LINKER_OPTIONS",
    [68] = "CL_INVALID_DEVICE_PARTITION_COUNT",
    [69] = "CL_INVALID_PIPE_SIZE",
    [70] = "CL_INVALID_DEVICE_QUEUE"
  };
  static char error_string[256];

  sprintf(error_string,"%d[%s]",e,(-e >= 0 && -e < 70 && error_description[-e] != NULL) ? error_description[-e] : "UNKNOWN");
  return &error_string[0];
}


//
// Main program
//

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Please insert a max B parameter, none inserted\nUsage -> ./pierce_expansion <uint>\n");
    return 1;
  }
  int maxB = atoi(argv[1]);

  if (maxB < 1) {
    printf("B param must be larger than 0!\n");
    return 1;
  }

  int i;

  //
  // read the OpenCL kernel source code (this could be a string in our source code, but it is easier during code development to read it from a file)
  //
  char open_cl_source_code[8192];
  size_t open_cl_source_code_size;
  FILE *fp;

  fp = fopen("openCL_Pierce.cl","r");
  if(fp == NULL) {
    perror("fopen()");
    exit(1);
  }
  open_cl_source_code_size = fread((void *)&open_cl_source_code[0],sizeof(char),sizeof(open_cl_source_code),fp);
  if(open_cl_source_code_size < (size_t)1 || open_cl_source_code_size >= sizeof(open_cl_source_code)) {
    fprintf(stderr,"fread(): the OpenCL kernel code is either too small or too large\n");
    exit(1);
  }
  fclose(fp);

  //
  // get the first OpenCL platform ID
  //
  cl_uint num_platforms;
  cl_platform_id platform_id[1];

  cl_call( clGetPlatformIDs , (1,&platform_id[0],&num_platforms) );
  if(num_platforms < 1) {
    fprintf(stderr,"No OpenCL platform\n");
    exit(1);
  }
  if(num_platforms > 1)
    fprintf(stderr,"Warning: more than one OpenCL platform found (using the first one)\n");

  //
  // get information about the first OpenCL device (use CL_DEVICE_TYPE_CPU or CL_DEVICE_TYPE_GPU to force a specific device type)
  //
  cl_uint num_devices;
  cl_device_id device_id[1];

  cl_call( clGetDeviceIDs , (platform_id[0],CL_DEVICE_TYPE_DEFAULT,1,&device_id[0],&num_devices) );
  if(num_devices < 1) {
    fprintf(stderr,"No OpenCL device\n");
    exit(1);
  }
  if(num_devices > 1)
    fprintf(stderr,"Warning: more than one OpenCL device found (using the first one)\n");

  //
  // create an OpenCL context
  //
  cl_context context;

  cl_call_alt( context = clCreateContext , (NULL,1,&device_id[0],NULL,NULL,&e) );

  //
  // create an OpenCL command queue
  //
  cl_command_queue command_queue;

  cl_call_alt( command_queue = clCreateCommandQueue, (context,device_id[0],0,&e) );


  size_t thread_count;
  cl_call( clGetDeviceInfo , (device_id[0],CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(thread_count),(void *)&thread_count,NULL) );

  int maxIter = (maxB / thread_count) + 1;

  //
  // create a memory area in device memory where the "Hello, world!" string will be placed
  //
  char host_buffer[thread_count];
  cl_mem device_buffer;
  int buffer_size;

  buffer_size = (int)sizeof(host_buffer);
  cl_call_alt( device_buffer = clCreateBuffer, (context,CL_MEM_READ_WRITE,(size_t)buffer_size,NULL,&e) );

  //
  // transfer the OpenCL code to the OpenCL context
  //
  char *program_lines[1];
  size_t program_line_lengths[1];
  cl_program program;

  program_lines[0] = &open_cl_source_code[0];
  program_line_lengths[0] = open_cl_source_code_size;
  cl_call_alt( program = clCreateProgramWithSource, (context,1,(const char **)&program_lines[0],&program_line_lengths[0],&e) );

  //
  // compile the OpenCL code and get the pierce() kernel handle
  //
  cl_kernel kernel;

  cl_call( clBuildProgram , (program,1,&device_id[0],NULL,NULL,NULL) );
  cl_call_alt( kernel = clCreateKernel , (program,"pierce",&e) );



  //
  // run the kernel (set its arguments iand launch grid first)
  //
  // we are launching sizeof(host_buffer) threads here; each thread initializes only one byte of the device_buffer array
  // as we will be launching a single kernel, there is no need to specify the events this kernel has to wait for
  //
  unsigned int iter = 0;
  unsigned int largestC = 0;

  printf(" ╭───────────────────────────────────────────────────╮\n");
  printf(" │------------------ Buffer Values ------------------│\n");

  printf(" ├──────────────────────────────────────────────────┤\n");
  for (; iter < maxIter; iter++) {
    size_t total_work_size[1],local_work_size[1]; // number of threads
    cl_event pierce_kernel_done[1];

    cl_call( clSetKernelArg , (kernel,0,sizeof(cl_mem),(void *)&device_buffer) );
    cl_call( clSetKernelArg , (kernel,1,sizeof(int),&buffer_size) );
    cl_call( clSetKernelArg , (kernel,2,sizeof(int),&iter) );
    cl_call( clSetKernelArg , (kernel,3,sizeof(int),&thread_count) );

    total_work_size[0] = (size_t)buffer_size; // the total number of threads (one dimension)
    local_work_size[0] = (size_t)buffer_size; // the number of threads in each work group (in this small example, all of them)
    cl_call( clEnqueueNDRangeKernel , (command_queue,kernel,1,NULL,&total_work_size[0],&local_work_size[0],0,NULL,&pierce_kernel_done[0]) );

    //
    // copy the buffer form device memory to CPU memory (copy only after the kernel has finished and block host execution until the copy is completed)
    //
    cl_call( clEnqueueReadBuffer , (command_queue,device_buffer,CL_TRUE,0,(size_t)buffer_size,(void *)host_buffer,1,&pierce_kernel_done[0],NULL) );
  
    //
    // display host_buffer
    //
    for(i = 0; i < buffer_size / sizeof(char); i++) {
      unsigned int currThread = i + iter*thread_count;

      if (host_buffer[i] > largestC && currThread < maxB) {
        largestC = host_buffer[i];
        printf(" │ Thread: %6d    │    Val: %4d    │    Hex:  %04X │\n", currThread, host_buffer[i], host_buffer[i] & 0xFFFF);
      }

    }
  }
  printf(" ╰──────────────────────────────────────────────────╯\n");
  printf(" ╭──────────────────────────────────────────────────╮\n");
  printf(" │   Number of Iterations: %3d with %4d threads    │\n", iter, (int)thread_count);
  printf(" ╰──────────────────────────────────────────────────╯\n");

  //
  // clean up (optional)
  //
  cl_call( clFlush , (command_queue) );
  cl_call( clFinish , (command_queue) );
  cl_call( clReleaseKernel , (kernel) );
  cl_call( clReleaseProgram , (program) );
  cl_call( clReleaseMemObject , (device_buffer) );
  cl_call( clReleaseCommandQueue , (command_queue) );
  cl_call( clReleaseContext , (context) );

  //
  // all done!
  //
  return 0;
}
