# Jomon

Jomon pottery-/Sheikah-inspired pattern image generator

## Todos

- [x] Allow command line arguments to be passed, with defaults for not passing them
  - [x] Height and width
  - [x] Dark and Light colors
  - [x] Output file name
      - [ ] (Maybe) also allow outputting to stdout
  - [x] Stroke width
  - [x] Density minimum and maximum (range of how many circles to include)  
  - [x] Random seed for srand and rand functions
- [x] Implement random generator of circles (position and radius/diameter)
- [x] Implement shader-like function for determining the pixels
  - [x] Determine a given pixel's distance to the center of a circle and if the pixel is within that circle,
    - [x] If within the circle, use distance to determine pixel color
    - [x] If not within the circle, use x value to determine column and color based on column
- [ ] Makefile compiler flags
  - [ ] Default macro definitions in Makefile as variables for easy redefinition when compiling

## What the program *should* be structured like

- Parse CLI flags and use defaults for missing flags
- Call srand with CLI seed or current time as default
- Generate random number of circles with random positions and size (number of bands)
- Sort circles by size
- Set up buffer for storing pixels
- Run pixel function for each pixel
- Generate final PPM file data from buffer and some metadata
- Put PPM file data into where it belongs
- Print to stdout the filename (unless output is put to stdout)