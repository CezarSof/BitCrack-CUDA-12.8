NAME = CudaKeySearchDevice

# Source files
CPPSRC := $(wildcard *.cpp)
CUSRC  := $(wildcard *.cu)
OBJ := $(CPPSRC:.cpp=.o) $(CUSRC:.cu=.o)

# Compiler settings
CXX ?= g++
NVCC ?= nvcc

# CUDA Architecture for RTX 4050 (Ada Lovelace)
NVCCFLAGS = -gencode arch=compute_89,code=sm_89 \
            -std=c++17 -Xptxas "-v" -Xcompiler "-O3 -std=c++17" \
            -use_fast_math -maxrregcount=32 -flto

# C++ compiler flags
CXXFLAGS = -O3 -std=c++17 -flto -DBUILD_CUDA

# Include paths
INCLUDE = -I$(CUDA_INCLUDE) -I$(CUDA_MATH)

# CUDA libraries
LDFLAGS = -lcudart -lcudadevrt

# Output directory for libraries
LIBDIR := ./lib

# Default target
all: cuda

cuda: $(OBJ)
	@mkdir -p $(LIBDIR)

	# Link device object files
	$(NVCC) -dlink -o cuda_libs.o $(CUSRC:.cu=.o) $(LDFLAGS)

	# Create static library
	ar rvs $(LIBDIR)/lib$(NAME).a $(OBJ)

# Compile C++ files
%.o: %.cpp
	$(CXX) -c $< $(INCLUDE) $(CXXFLAGS) -o $@

# Compile CUDA files
%.o: %.cu
	$(NVCC) -c $< -o $@ $(NVCCFLAGS) -rdc=true $(INCLUDE)

# Cleanup
clean:
	rm -rf *.o cuda_libs.o $(LIBDIR)/lib$(NAME).a
