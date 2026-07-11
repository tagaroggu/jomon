CC := clang

Major := 0
Minor := 0
Patch := 0
Version := $(Major).$(Minor).$(Patch)

jomon: main.c
	$(CC) main.c -o jomon

test: test.c main.c
	$(CC) -DJOMON_LIBRARY main.c test.c -o test

.PHONY: clean
clean:
	rm jomon test

