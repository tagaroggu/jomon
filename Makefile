CC := clang

Major := 0
Minor := 0
Patch := 0
Version := $(Major).$(Minor).$(Patch)

DefaultWidth := 1920
DefaultHeight := 1080
DefaultStrokeWidth := 3
DefaultDarkColor := "(struct Color) { 0x54, 0x44, 0x2B }"
DefaultLightColor := "(struct Color) { 0xA9, 0x71, 0x4B }"
DefaultMinDensity := 25
DefaultMaxDensity := 100
# DefaultSeed := 
# DefaultFileName :=
DefaultMinRadius := 5
DefaultMaxRadius := 75

Defaults := -DDEFAULT_WIDTH=$(DefaultWidth) -DDEFAULT_HEIGHT=$(DefaultHeight) -DDEFAULT_STROKE_WIDTH=$(DefaultStrokeWidth) -DDEFAULT_BG_COLOR=$(DefaultDarkColor) -DDEFAULT_FG_COLOR=$(DefaultLightColor) -DDEFAULT_MIN_DENSITY=$(DefaultMinDensity) -DDEFAULT_MAX_DENSITY=$(DefaultMaxDensity) -DDEFAULT_MIN_RADIUS=$(DefaultMinRadius) -DDEFAULT_MAX_RADIUS=$(DefaultMaxRadius)

CFLAGS := $(Defaults)

jomon: main.c
	$(CC) $(CFLAGS) main.c -o jomon

test: test.c main.c
	$(CC) $(CFLAGS) -DJOMON_LIBRARY test.c -o test

.PHONY: clean
clean:
	rm jomon test

