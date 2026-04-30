# configuration

TEST_CASE = juliaset
BUILD_DIR = ../../build/$(TEST_CASE)


# default benchmark parameters

NX    = 512
NY    = 512

PARAMETERS = ${NX} ${NY}


# all

targets = $(BUILD_DIR)/$(TEST_CASE)-base


.PHONY: all
all: mk-target-dir $(targets)


mk-target-dir:
	mkdir -p $(BUILD_DIR)


# build rules

$(BUILD_DIR)/$(TEST_CASE)-base: $(TEST_CASE)-base.cpp ../lodepng/lodepng.h ../lodepng/lodepng.cpp
	g++ -O3 -march=native -std=c++17 \
	$(TEST_CASE)-base.cpp ../lodepng/lodepng.cpp \
	-o $(BUILD_DIR)/$(TEST_CASE)-base

# aliases without build directory

.PHONY: $(TEST_CASE)-base
$(TEST_CASE)-base: $(BUILD_DIR)/$(TEST_CASE)-base


# automated benchmark target

.PHONY: bench
bench: all
	@echo "Base:"
	$(BUILD_DIR)/$(TEST_CASE)-base $(PARAMETERS)
	@echo ""


# clean target

.PHONY: clean
clean:
	rm $(targets)
