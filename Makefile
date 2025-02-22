# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
INCLUDE_DIR = include
CPPFLAGS = -I$(INCLUDE_DIR)  # Include directory for header files
TESTFLAG = -DTEST
ifeq ($(DEBUG), true)
	CXXFLAGS += $(TESTFLAG) -g
endif

# Directories
LIB_DIR = lib
TEST_DIR = tests
BUILD_DIR = build

# Object files for lib
LIB_SRCS = $(shell find $(LIB_DIR) -name "*.cpp")
LIB_OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(LIB_SRCS)))))

# Object files for test
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.cpp")
TEST_BINS = $(TEST_DIR)/bin/$(basename $(notdir $(TEST_SRCS)))
TEST_OUT = $(TEST_DIR)/out/$(basename $(notdir $(TEST_SRCS)))

# Default rule (build main)
all: $(MAIN_TARGET)

$(LIB_OBJS): $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(TEST_BINS): $(TEST_SRCS) $(LIB_OBJS)
	$(info $(TEST_SRCS))
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TESTFLAG) $(LIB_OBJS) $< -o $@

# Run tests
$(TEST_OUT): $(TEST_BINS)
	./$< > $@

lib: $(LIB_OBJS)

tests: $(TEST_OUT)

# Clean rule
clean:
	find . -name "*.o" -delete
	rm -fr tests/bin/*
	rm -fr tests/out/*

