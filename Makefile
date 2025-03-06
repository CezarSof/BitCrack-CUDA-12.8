# Define base paths
CUR_DIR := $(shell pwd)
DIRS := util AddressUtil CmdParse CryptoUtil KeyFinderLib CudaKeySearchDevice cudaMath cudaUtil secp256k1lib Logger

INCLUDE := $(foreach d, $(DIRS), -I$(CUR_DIR)/$d)

# Define library and binary directories
LIBDIR := $(CUR_DIR)/lib
BINDIR := $(CUR_DIR)/bin
LIBS := -L$(LIBDIR)

# C++ options
CXX := g++
CXXFLAGS := -O2 -std=c++17

# CUDA configuration (optimized for RTX 4050)
COMPUTE_CAP := 86
NVCC := nvcc
NVCCFLAGS := -std=c++17 -gencode=arch=compute_${COMPUTE_CAP},code=sm_${COMPUTE_CAP} -Xptxas="-v" -Xcompiler "${CXXFLAGS}"
CUDA_HOME := /usr/local/cuda-12.8
CUDA_LIB := ${CUDA_HOME}/lib64
CUDA_INCLUDE := ${CUDA_HOME}/include
CUDA_MATH := $(CUR_DIR)/cudaMath

# Export environment variables
export INCLUDE LIBDIR BINDIR NVCC NVCCFLAGS LIBS CXX CXXFLAGS CUDA_LIB CUDA_INCLUDE CUDA_MATH BUILD_CUDA=1

# Define build targets
TARGETS := $(addprefix dir_, AddressUtil CmdParse CryptoUtil KeyFinderLib CudaKeySearchDevice cudaUtil secp256k1lib Logger AddrGen)

# Ensure bin and lib directories exist
$(BINDIR) $(LIBDIR):
	mkdir -p $@

# Default target to build
all: $(BINDIR) $(LIBDIR) $(TARGETS)

# Build dependencies and directories
define build_dir
$(1): $(addprefix dir_, $(2))
	$(MAKE) --directory $(1) $(MAKEFLAGS)
endef

$(foreach dir, $(DIRS), $(eval $(call build_dir,$(dir),)))

# Clean up build artifacts
clean:
	@echo "Cleaning build files..."
	$(foreach dir, $(DIRS), $(MAKE) --directory $(dir) clean;)
	rm -rf $(LIBDIR) $(BINDIR)

# Allow parallel builds
.PHONY: all clean $(DIRS)
