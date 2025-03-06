CPPSRC := $(wildcard *.cpp)

all:
ifeq ($(BUILD_CUDA), 1)
	${CXX} -DBUILD_CUDA -o cuKeyFinder.bin ${CPPSRC} ${INCLUDE} -I${CUDA_INCLUDE} ${CXXFLAGS} ${LIBS} -L${CUDA_LIB} -lkeyfinder -laddressutil -lsecp256k1 -lcryptoutil -lsecp256k1 -lcudautil -llogger -lutil -lCudaKeySearchDevice -lcudadevrt -lcudart -lcmdparse
	mkdir -p $(BINDIR)
	cp cuKeyFinder.bin $(BINDIR)/cuBitCrack
endif

clean:
	rm -rf cuKeyFinder.bin
