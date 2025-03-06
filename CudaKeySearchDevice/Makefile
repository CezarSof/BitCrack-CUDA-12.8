NAME = CudaKeySearchDevice

# Source files
CPPSRC := $(wildcard *.cpp)
CUSRC  := $(wildcard *.cu)
OBJ := $(CPPSRC:.cpp=.o) $(CUSRC:.cu=.o)

# Compiler
CXX ?= g++
NVCC ?= nvcc

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

cuda: $(OBJ)
	@mkdir -p $(LIBDIR)

	# Link device object files if CUDA is enabled
ifeq ($(BUILD_CUDA), 1)
	$(NVCC) -dlink -o cuda_libs.o $(CUSRC:.cu=.o) -lcudadevrt -lcudart
endif

	# Create static library
	ar rvs $(LIBDIR)/lib$(NAME).a $(OBJ)

# Compile C++ files
%.o: %.cpp
	$(CXX) -c $< $(INCLUDE) $(CXXFLAGS) -o $@

# Compile CUDA files
%.o: %.cu
	$(NVCC) -c $< -o $@ $(NVCCFLAGS) -rdc=true $(INCLUDE)

clean:
	rm -rf *.o cuda_libs.o $(LIBDIR)/lib$(NAME).a
