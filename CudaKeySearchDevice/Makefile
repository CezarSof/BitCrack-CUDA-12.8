NAME = CudaKeySearchDevice

# Source files
CPPSRC := $(wildcard *.cpp)
CUSRC  := $(wildcard *.cu)

# Check if we should build CUDA
ifeq ($(BUILD_CUDA), 1)
    # Set CUDA architecture for RTX 4050 (Ada Lovelace architecture)
    NVCCFLAGS = -gencode arch=compute_89,code=sm_89 \
                -std=c++17 -Xptxas "-v" -Xcompiler "-O3 -std=c++17" \
                -use_fast_math -maxrregcount=32 -flto

    # Set C++ flags for CUDA build
    CXXFLAGS += -DBUILD_CUDA -O3 -std=c++17 -flto

    # Include paths for CUDA
    INCLUDE = -I$(CUDA_INCLUDE) -I$(CUDA_MATH)

    # Library paths for CUDA
    LDFLAGS = -lcudart -lcudadevrt
else
    # Set C++ flags when not building CUDA
    CXXFLAGS += -O3 -std=c++17 -flto
    INCLUDE = 
    LDFLAGS = 
endif

# Output directory for libraries (make sure LIBDIR is set correctly)
LIBDIR := ./lib

# Default target
all: cuda

cuda:
	# Compile C++ source files
	for file in ${CPPSRC}; do \
		${CXX} -c $$file ${INCLUDE} ${CXXFLAGS} -o $$file.o; \
	done

	# Only compile CUDA source files if BUILD_CUDA=1
	ifeq ($(BUILD_CUDA), 1)
		for file in ${CUSRC}; do \
			${NVCC} -c $$file -o $$file.o ${NVCCFLAGS} -rdc=true ${INCLUDE}; \
		done
		# Link device object files into one file
		${NVCC} -dlink -o cuda_libs.o *.cu.o -lcudadevrt -lcudart
	endif

	# Create static library
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

clean:
	rm -f *.o *.cu.o *.a cuda_libs.o