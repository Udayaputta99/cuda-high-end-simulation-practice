# configuration

TEST_CASE = juliaset
BUILD_DIR = ./build/$(TEST_CASE)

# default benchmark parameters

NX    = 800
NY    = 800
CR    = -0.8
CI    = 0.156
RGB   = "rgb" #"bw" or "rgb"
PARAMETERS = ${NX} ${NY} ${CR} ${CI} ${RGB} 
CUDA_ARCH = "sm_86"

# all
targets = $(BUILD_DIR)/$(TEST_CASE)-base $(BUILD_DIR)/$(TEST_CASE)-cuda


.PHONY: all
all: mk-target-dir $(targets)

mk-target-dir:
	mkdir -p $(BUILD_DIR)

# build rules

$(BUILD_DIR)/$(TEST_CASE)-base: $(TEST_CASE)-base.cpp ./lodepng/lodepng.h ./lodepng/lodepng.cpp 
	g++ -O3 -march=native -std=c++17 \
	$(TEST_CASE)-base.cpp ./lodepng/lodepng.cpp \
	-o $(BUILD_DIR)/$(TEST_CASE)-base
$(BUILD_DIR)/$(TEST_CASE)-cuda: $(TEST_CASE)-cuda.cu ./lodepng/lodepng.h ./lodepng/lodepng.cpp ./cudautils.h
	nvcc -O3 -std=c++17 -arch=$(CUDA_ARCH) $(TEST_CASE)-cuda.cu ./lodepng/lodepng.cpp \
 	-o $(BUILD_DIR)/$(TEST_CASE)-cuda

# aliases without build directory

.PHONY: $(TEST_CASE)-base
$(TEST_CASE)-base: $(BUILD_DIR)/$(TEST_CASE)-base

.PHONY: $(TEST_CASE)-cuda
$(TEST_CASE)-cuda: $(BUILD_DIR)/$(TEST_CASE)-cuda

# automated benchmark target

.PHONY: base
base: all
	@echo "Base:"
	$(BUILD_DIR)/$(TEST_CASE)-base $(PARAMETERS)
	@echo ""

.PHONY: cuda
cuda: all
	@echo "Cuda:"
	$(BUILD_DIR)/$(TEST_CASE)-cuda $(PARAMETERS)
	@echo ""

# clean target

.PHONY: clean
clean:
	rm $(targets)
