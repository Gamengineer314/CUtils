INCLUDES=-Iinclude
TESTS=$(wildcard test/test_*.c)
TESTS_CPP=$(wildcard test_cpp/test_*.cpp)

all: bin/main $(TESTS:test/test_%.c=bin/test_%) $(TESTS_CPP:test_cpp/test_%.cpp=bin/cpp_%)

bin/main: main.c algorithms.h include/*.h
	@echo "Compiling main..."
	@gcc -Wall $(INCLUDES) main.c algorithms.c -lthrow -o $@

bin/test_%: test/test_%.c include/%.h test/algorithms.h
	@echo "Compiling $<..."
	@gcc -O3 -Wall $(INCLUDES) $< test/algorithms.c -lthrow -lbenchmark -o $@

bin/cpp_%: test_cpp/test_%.cpp
	@echo "Compiling $<..."
	@g++ -O3 -Wall $(INCLUDES) $< -lbenchmark -o $@

clean:
	@rm -f bin/* obj/*

.PHONY: clean