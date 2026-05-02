# configuration

TEST_CASE = juliaset
BUILD_DIR = ../../build/$(TEST_CASE)


# default benchmark parameters

NX    = 6720
NY    = 6720
CR    = -0.8
CI    = 0.2

PARAMETERS = ${NX} ${NY} ${CR} ${CI}


# all

targets = $(BUILD_DIR)/$(TEST_CASE)-base $(BUILD_DIR)/$(TEST_CASE)-cuda


.PHONY: all
all: mk-target-dir $(targets)


mk-target-dir:
	mkdir -p $(BUILD_DIR)


# build rules

$(BUILD_DIR)/$(TEST_CASE)-base: $(TEST_CASE)-base.cpp ../lodepng/lodepng.h ../lodepng/lodepng.cpp ../CImg/CImg.h ../util.h
	g++ -O3 -march=native -std=c++17 \
	$(TEST_CASE)-base.cpp ../lodepng/lodepng.cpp \
	-o $(BUILD_DIR)/$(TEST_CASE)-base -lz -lX11

$(BUILD_DIR)/$(TEST_CASE)-cuda: $(TEST_CASE)-cuda.cu ../lodepng/lodepng.h ../lodepng/lodepng.cpp ../CImg/CImg.h ../util.h
	nvcc -O3 -std=c++17 -arch=sm_86 \
	$(TEST_CASE)-cuda.cu ../lodepng/lodepng.cpp \
	-o $(BUILD_DIR)/$(TEST_CASE)-cuda -lz -lX11

# aliases without build directory

.PHONY: $(TEST_CASE)-base
$(TEST_CASE)-base: $(BUILD_DIR)/$(TEST_CASE)-base

.PHONY: $(TEST_CASE)-cuda
$(TEST_CASE)-cuda: $(BUILD_DIR)/$(TEST_CASE)-cuda

# automated benchmark target

.PHONY: bench
bench: all
	@echo "Base:"
	$(BUILD_DIR)/$(TEST_CASE)-base $(PARAMETERS)
	@echo ""
	@echo "Cuda:"
	$(BUILD_DIR)/$(TEST_CASE)-cuda $(PARAMETERS)
	@echo ""

bench-%: all
	@echo "Cuda:"
	$(BUILD_DIR)/$(TEST_CASE)-$* $(PARAMETERS)
	@echo ""

# clean target

.PHONY: clean
clean:
	rm $(targets)
