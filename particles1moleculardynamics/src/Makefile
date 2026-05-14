# configuration

TEST_CASE = simulator
BUILD_DIR = ../build/$(TEST_CASE)


# all

targets = $(BUILD_DIR)/$(TEST_CASE)-base


.PHONY: all
all: mk-target-dir $(targets)


mk-target-dir:
	mkdir -p $(BUILD_DIR)


# build rules

$(BUILD_DIR)/$(TEST_CASE)-base: $(TEST_CASE)-base.cpp ../include/rapidcsv.h ../include/json.hpp ../include/ConfigReader.hpp ../include/ParticleIO.hpp ../include/CellList.hpp ../include/ForceCalculator.hpp ../include/Integrator.hpp ../include/PeriodicBoundary.hpp ../include/Vec.hpp  
	g++ -O3 -march=native -std=c++17 -o $(BUILD_DIR)/$(TEST_CASE)-base $(TEST_CASE)-base.cpp 

# aliases without build directory

.PHONY: $(TEST_CASE)-base
$(TEST_CASE)-base: $(BUILD_DIR)/$(TEST_CASE)-base

# automated benchmark target

.PHONY: bench
bench: all
	@echo "Base:"
	$(BUILD_DIR)/$(TEST_CASE)-base
	@echo ""

# clean target

.PHONY: clean
clean:
	rm $(targets)