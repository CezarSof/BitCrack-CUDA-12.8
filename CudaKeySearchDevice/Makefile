NAME = CudaKeySearchDevice

# Source files
CPPSRC := $(wildcard *.cpp)
CUSRC := $(wildcard *.cu)

# Object files
CPP_OBJ := $(CPPSRC:.cpp=.o)
CU_OBJ := $(CUSRC:.cu=.o)

# Flags for NVCC and CXX
CXXFLAGS += ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS}
NVCCFLAGS += -gencode arch=compute_89,code=sm_89 -use_fast_math ${INCLUDE} -I${CUDA_INCLUDE} -I${CUDA_MATH}

# Library Directory
LIBRARY := ${LIBDIR}/lib$(NAME).a

# Default target
all: $(LIBRARY)

# Compile .cpp files
$(CPP_OBJ): %.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Compile .cu files
$(CU_OBJ): %.o: %.cu
	$(NVCC) -c $< -o $@ $(NVCCFLAGS) -rdc=true

# Link .cu object files
$(LIBRARY): $(CPP_OBJ) $(CU_OBJ)
	$(NVCC) -dlink -o cuda_libs.o $(CU_OBJ) -lcudadevrt -lcudart
	ar rvs $(LIBRARY) $(CPP_OBJ) $(CU_OBJ)

# Clean up
clean:
	rm -f $(CPP_OBJ) $(CU_OBJ) $(LIBRARY) cuda_libs.o
