# configuration

TEST_CASE = simulator
BUILD_DIR = ../build/$(TEST_CASE)


# all

targets = $(BUILD_DIR)/$(TEST_CASE)-cuda


.PHONY: all
all: mk-target-dir $(targets)


mk-target-dir:
	mkdir -p $(BUILD_DIR)


# build rules

$(BUILD_DIR)/$(TEST_CASE)-cuda: $(TEST_CASE)-cuda.cu ../include/rapidcsv.h ../include/json.hpp ../include/ConfigReader.hpp ../include/ParticleIO.hpp
	nvcc -O3 -std=c++17 \
		-gencode arch=compute_75,code=sm_75 \
		-gencode arch=compute_80,code=sm_80 \
		-gencode arch=compute_86,code=sm_86 \
		-o $(BUILD_DIR)/$(TEST_CASE)-cuda $(TEST_CASE)-cuda.cu

# aliases without build directory

.PHONY: $(TEST_CASE)-cuda
$(TEST_CASE)-cuda: $(BUILD_DIR)/$(TEST_CASE)-cuda

# automated benchmark target

.PHONY: bench
bench: all
	@echo "Periodic boundary and cells list CUDA:"
	$(BUILD_DIR)/$(TEST_CASE)-cuda
	@echo ""

# clean target

.PHONY: clean
clean:
	rm $(targets)