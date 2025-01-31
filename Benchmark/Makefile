INCLUDES=-Iinclude

bin/main: obj/main.o obj/benchmark.o
	@echo "Linking..."
	@gcc -Wall $^ -lm -lthrow -o $@

obj/main.o: main.c
	@echo "Compiling main..."
	@gcc -c -Wall $(INCLUDES) $< -o $@

obj/benchmark.o: src/benchmark.c
	@echo "Compiling benchmark..."
	@gcc -c -Wall -fpic $(INCLUDES) $< -o $@

obj/libbenchmark.so: obj/benchmark.o
	@echo "Compiling shared library..."
	@gcc -fpic -shared $^ -lm -lthrow -o $@

clean:
	@rm -f bin/* obj/*

.PHONY: clean