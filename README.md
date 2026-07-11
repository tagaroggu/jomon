# Jomon

Jomon pottery-/Sheikah-inspired pattern image generator

## Todos

- [ ] Allow command line arguments to be passed, with defaults for not passing them
  - [ ] Height and width
  - [ ] Dark and Light colors
  - [ ] Output file name
    - [ ] (Maybe) also allow outputting to stdout
  - [ ] Random seed for srand and rand functions
- [ ] Implement random generator of circles (position and radius/diameter)
- [ ] Implement shader-like function for determining the pixels
  - [ ] Determine a given pixel's distance to the center of a circle and if the pixel is within that circle,
    - [ ] If within the circle, use distance to determine pixel color
    - [ ] If not within the circle, use x value to determine column and color based on column
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
