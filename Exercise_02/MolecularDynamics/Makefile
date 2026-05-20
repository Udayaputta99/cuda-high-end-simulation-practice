# configuration

TEST_CASE = particleSim
BUILD_DIR = ./build/$(TEST_CASE)

# default benchmark parameters

CUDA_ARCH = sm_86

# all
targets = $(BUILD_DIR)/$(TEST_CASE)-base $(BUILD_DIR)/$(TEST_CASE)-cuda


.PHONY: all mk-target-dir clean base cuda $(TEST_CASE)-base $(TEST_CASE)-cuda
all: mk-target-dir $(targets)

mk-target-dir:
	mkdir -p $(BUILD_DIR)

# build rules

$(BUILD_DIR)/$(TEST_CASE)-base: src/$(TEST_CASE).cpp src/domain.h src/force.h src/particle.h src/util.h src/vtuWriter.h
	g++ -O3 -march=native -std=c++17 src/$(TEST_CASE).cpp -o $(BUILD_DIR)/$(TEST_CASE)-base

$(BUILD_DIR)/$(TEST_CASE)-cuda: src_cuda/$(TEST_CASE).cu src_cuda/force.h src_cuda/cudautils.h src_cuda/domain.h src_cuda/particle.h src_cuda/util.h src_cuda/vtuWriter.h
	nvcc -O3 -std=c++17 -arch=$(CUDA_ARCH) src_cuda/$(TEST_CASE).cu -o $(BUILD_DIR)/$(TEST_CASE)-cuda

# aliases without build directory

$(TEST_CASE)-base: $(BUILD_DIR)/$(TEST_CASE)-base

$(TEST_CASE)-cuda: $(BUILD_DIR)/$(TEST_CASE)-cuda

# automated benchmark target

base: mk-target-dir $(BUILD_DIR)/$(TEST_CASE)-base
	@echo "Base:"
	$(BUILD_DIR)/$(TEST_CASE)-base 
	@echo ""

cuda: mk-target-dir $(BUILD_DIR)/$(TEST_CASE)-cuda
	@echo "Cuda:"
	$(BUILD_DIR)/$(TEST_CASE)-cuda 
	@echo ""

# clean target

clean:
	rm -rf $(BUILD_DIR)