#
# TOS, November 2020
#
# ACA 2020/2021
#
# the CUDA stuff is assumed to be installed in /usr/local/cuda
# the CUDA device is assumed to have architecture sm_75 (for a GeForce GTX 1660 Ti)
#

CUDA_ARCH = sm_75


clean:
        rm -f open_cl_hello cuda_hello cuda_hello.cubin

all:    open_cl_hello cuda_hello


open_cl_hello:          open_cl_hello.c
        cc -Wall -O2 -std=c99 open_cl_hello.c -o open_cl_hello -L/usr/local/cuda/lib64 -lOpenCL


cuda_hello:             cuda_hello.c cuda_hello.cubin
        cc -Wall -O2 -std=c99 -I/usr/local/cuda/include cuda_hello.c -o cuda_hello -L/usr/local/cuda/lib64 -lcuda

cuda_hello.cubin:       cuda_hello.cu
        nvcc -arch=$(CUDA_ARCH) --compiler-options -O2,-Wall -I/usr/local/cuda/include --cubin cuda_hello.cu -o cuda_hello.cubin
