NAME = CudaKeySearchDevice
CPPSRC := $(wildcard *.cpp)
CUSRC := $(wildcard *.cu)

# Add the necessary flags for RTX 4050 architecture
NVCCFLAGS += -gencode arch=compute_89,code=sm_89

all: cuda

cuda:
	# Compile .cpp files
	for file in ${CPPSRC} ; do\
		${CXX} -c $$file ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS};\
	done

	# Compile .cu files with NVCC and RTX 4050 support
	for file in ${CUSRC} ; do\
		${NVCC} -c $$file -o $$file".o" ${NVCCFLAGS} -rdc=true ${INCLUDE} -I${CUDA_INCLUDE} -I${CUDA_MATH};\
	done

	# Link the CUDA object files
	${NVCC} -dlink -o cuda_libs.o *.cu.o -lcudadevrt -lcudart

	# Create the static library
	ar rvs ${LIBDIR}/lib$(NAME).a *.o

clean:
	rm -f *.o *.cu.o
	rm -f *.a
