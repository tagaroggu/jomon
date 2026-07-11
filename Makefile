CC := clang

Major := 0
Minor := 0
Patch := 0
Version := $(Major).$(Minor).$(Patch)

jomon: main.c
	$(CC) main.c -o jomon

.PHONY: clean
clean:
	rm jomon


