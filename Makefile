CC := clang

jomon: main.c
	$(CC) main.c -o jomon

.PHONY: clean
clean:
	rm jomon


