NAME = CudaKeySearchDevice
CPPSRC := $(wildcard *.cpp)
CUSRC  := $(wildcard *.cu)

# Set CUDA architecture for RTX 4050 (Ada Lovelace architecture)
NVCCFLAGS = -gencode arch=compute_89,code=sm_89 \
            -std=c++17 -Xptxas "-v" -Xcompiler "-O3 -std=c++17"  # Added -O3 for CUDA

# Add general optimization flags for C++ source files
CXXFLAGS += -O3  # Added -O3 for the C++ compiler to optimize for maximum performance

all: cuda

cuda:
	# Compile C++ source files with optimization level -O3
	for file in ${CPPSRC}; do \
		${CXX} -c $$file ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS}; \
	done

	# Compile CUDA source files with optimization level -O3 for CUDA code
	for file in ${CUSRC}; do \
		${NVCC} -c $$file -o $$file".o" ${NVCCFLAGS} -rdc=true ${INCLUDE} -I${CUDA_INCLUDE} -I${CUDA_MATH}; \
	done

	# Link the object files and create the final static library
	${NVCC} -dlink -o cuda_libs.o *.cu.o -lcudadevrt -lcudart

	ar rvs ${LIBDIR}/lib$(NAME).a *.o

clean:
	rm -f *.o *.cu.o *.a
