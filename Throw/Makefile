INCLUDES=-Iinclude

bin/main: obj/main.o obj/throw.o
	@echo "Linking..."
	@gcc -Wall $^ -o $@

obj/main.o: main.c
	@echo "Compiling main..."
	@gcc -c -Wall $(INCLUDES) $< -o $@

obj/throw.o: src/throw.c
	@echo "Compiling throw..."
	@gcc -c -Wall -fpic $(INCLUDES) $< -o $@ || \
		(echo "Error ! Retrying without stacktrace..." ; gcc -c -Wall -fPIC $(INCLUDES) $< -DNO_STACKTRACE -o $@)

obj/libthrow.so: obj/throw.o
	@echo "Compiling shared library..."
	@gcc -fpic -shared $^ -o $@
#sudo cp obj/libthrow.so /usr/local/lib
#sudo cp include/throw.h /usr/local/include

clean:
	@rm -f bin/* obj/*

.PHONY: clean
