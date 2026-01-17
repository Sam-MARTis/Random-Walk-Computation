UNAME_S := $(shell uname)

CXX = g++
NVCC = /usr/local/cuda-12.8/bin/nvcc

CXXFLAGS += -Wall -O2
NVCC_FLAGS += -g -G -Xcompiler -Wall

# CUDA libs (needed)
LDFLAGS += -lcuda -lcudart -lcurand

# ---- VTK ----
VTK_INC = -I/usr/include/vtk-9.1

VTK_LIBS = \
  -lvtkFiltersCore-9.1 \
  -lvtkFiltersGeneral-9.1 \
  -lvtkCommonTransforms-9.1 \
  -lvtkCommonExecutionModel-9.1 \
  -lvtkCommonCore-9.1 \
  -lvtkCommonDataModel-9.1 \
  -lvtkRenderingCore-9.1 \
  -lvtkRenderingOpenGL2-9.1 \
  -lvtkRenderingAnnotation-9.1 \
  -lvtkRenderingFreeType-9.1 \
  -lvtkInteractionStyle-9.1




CXXFLAGS += $(VTK_INC)
LDFLAGS  += $(VTK_LIBS)

# ---- Objects ----
OBJS = main.o kernel.o

all: main.exe

# LINK WITH CXX (NOT NVCC)
main.exe: $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# C++ FILE (VTK lives here)
main.o: main.cpp kernel.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# CUDA FILE
kernel.o: kernel.cu kernel.hpp
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main.exe
