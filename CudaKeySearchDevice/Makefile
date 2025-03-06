NAME = CudaKeySearchDevice
CPPSRC := $(wildcard *.cpp)
CUSRC := $(wildcard *.cu)

# CUDA and compiler flags
CUDA_INCLUDE = /usr/local/cuda/include
CUDA_LIB = /usr/local/cuda/lib64
CUDA_MATH = /usr/local/cuda/include
CXX = g++
NVCC = nvcc
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra
NVCCFLAGS = -O3 -arch=sm_89 -use_fast_math -Xcompiler -fPIC
INCLUDE = -I$(CUDA_INCLUDE)

LIBDIR = ./libs

# Default target
all: cuda

# Compile C++ files
cuda:
	@echo "Compiling C++ files..."
	for file in ${CPPSRC} ; do \
		${CXX} -c $$file ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS}; \
	done

	@echo "Compiling CUDA files..."
	# Compile CUDA files with fast math
	for file in ${CUSRC} ; do \
		${NVCC} -c $$file -o $$file".o" ${NVCCFLAGS} -rdc=true ${INCLUDE} -I${CUDA_INCLUDE} -I${CUDA_MATH}; \
	done

	@echo "Linking CUDA files..."
	# Link CUDA files
	${NVCC} -dlink -o cuda_libs.o *.cu.o -lcudadevrt -lcudart

	@echo "Archiving static library..."
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

# Clean up build artifacts
clean:
	rm -f *.o *.cu.o
	rm -f ${LIBDIR}/lib$(NAME).a

.PHONY: all clean
