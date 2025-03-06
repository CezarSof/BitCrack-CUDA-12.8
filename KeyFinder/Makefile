NAME=keyfinder
CPPSRC:=$(wildcard *.cpp)
BINDIR=./bin

# CUDA 12.8 include and library paths (adjust based on your installation)
CUDA_INCLUDE=-I/usr/local/cuda-12.8/include
CUDA_LIB=-L/usr/local/cuda-12.8/lib64 -lcudart

# Libraries to link against
LIBS=-lkeyfinder -laddressutil -lsecp256k1 -lcryptoutil -llogger -lutil -lcmdparse

all:
ifeq ($(BUILD_CUDA), 1)
	${CXX} -DBUILD_CUDA -o cuKeyFinder.bin ${CPPSRC} ${INCLUDE} ${CUDA_INCLUDE} ${CXXFLAGS} ${LIBS} -L${CUDA_LIB} -lcudadevrt -lcudart -lCudaKeySearchDevice
	mkdir -p $(BINDIR)
	cp cuKeyFinder.bin $(BINDIR)/cuBitCrack
endif

clean:
	rm -rf cuKeyFinder.bin
