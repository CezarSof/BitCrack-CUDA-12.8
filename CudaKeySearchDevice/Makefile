NAME = CudaKeySearchDevice

CPPSRC := $(wildcard *.cpp)
CUSRC := $(wildcard *.cu)

# Define necessary flags
CUDA_VERSION := 12.8
CUDA_INCLUDE := /usr/local/cuda/include
CUDA_LIB := /usr/local/cuda/lib64
CUDA_MATH := /usr/local/cuda/include
CXXFLAGS := -O3 -g -G
NVCCFLAGS := -O3 -g -G --use_fast_math -arch=sm_89

# Include the appropriate directories
INCLUDE := -I$(CUDA_INCLUDE)

# All target: build the CUDA and C++ sources
all: cuda

# CUDA Compilation
cuda:
	# Compile C++ files
	for file in ${CPPSRC}; do \
		${CXX} -c $$file ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS}; \
	done

	# Compile CUDA files
	for file in ${CUSRC}; do \
		${NVCC} -c $$file -o $$file".o" ${NVCCFLAGS} ${INCLUDE} -I${CUDA_INCLUDE} -I${CUDA_MATH}; \
	done

	# Link CUDA object files
	${NVCC} -dlink -o cuda_libs.o *.cu.o -lcudadevrt -lcudart

	# Archive the static library
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

# Clean up generated files
clean:
	rm -f *.o *.cu.o
	rm -f *.a
