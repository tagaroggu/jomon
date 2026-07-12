/* TODO: Implementation of the whole thing. Write a function that is run for
 * each pixel that checks whether its within the area of a circle and if so its
 * distance to the center and therefore the pixel's color, otherwise which
 * column it is in and which color to match the column that it is in.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

// Width of the output image in pixels
#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 1920
#endif

// Height of the output image in pixels
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 1080
#endif

// WIdth of the lines
#ifndef DEFAULT_STROKE_WIDTH
#define DEFAULT_STROKE_WIDTH 3
#endif

// Darker, "background" color
#ifndef DEFAULT_BG_COLOR
#define DEFAULT_BG_COLOR (struct Color){0x54, 0x44, 0x2B}
#endif

// Lighter "foreground" color
#ifndef DEFAULT_FG_COLOR
#define DEFAULT_FG_COLOR (struct Color){0xA9, 0x71, 0x4B}
#endif

// Minimum density (count) of circles
#ifndef DEFAULT_MIN_DENSITY
#define DEFAULT_MIN_DENSITY 25
#endif

// Maximum density (count) of circles
#ifndef DEFAULT_MAX_DENSITY
#define DEFAULT_MAX_DENSITY 100
#endif

// Seed to use for random number generation
#ifndef DEFAULT_SEED
#define DEFAULT_SEED time(NULL)
#endif

// Name to output file to
#ifndef DEFAULT_FILE_NAME
#define DEFAULT_FILE_NAME ({ char timeStr[256]; \
  snprintf(timeStr, 256, "%lu", time(NULL)); \
  size_t strLen = strlen(timeStr) +  13; char *_temp = (char*)malloc(sizeof(char) * strLen); \
  snprintf(_temp, strLen, "./jomon-%s.ppm", timeStr); _temp; })
#endif

// Minimum radius (size) of circles
#ifndef DEFAULT_MIN_RADIUS
#define DEFAULT_MIN_RADIUS 5
#endif

// Maximum radius (size) of circles
#ifndef DEFAULT_MAX_RADIUS
#define DEFAULT_MAX_RADIUS 75
#endif

// Triple for colors
struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Size of a given area of pixels and pointer
// to the first of a buffer of the pixels
struct ColorBuffer {
  size_t width;
  size_t height;
  struct Color *pixels;
};

// Creates and returns a ColorBuffer with an allocated buffer for Colors
struct ColorBuffer createImageBuffer(size_t width, size_t height) {
  return (struct ColorBuffer){
    .width = width,
    .height = height,
    .pixels = (struct Color*)malloc(sizeof(struct Color) * width * height)
  };
}

// Gets a reference to a color by its index
struct Color *getColorAtIndex(struct ColorBuffer buffer, size_t index) {
  if (index >= buffer.width * buffer.height)
    return NULL;
  return &buffer.pixels[index];
}

// Gets a reference to a color by its coordinates
struct Color *getColorAtCoordinates(struct ColorBuffer buffer, size_t x, size_t y) {
  if (x < 0 || y < 0 || x >= buffer.width || y >= buffer.height)
    return NULL;
  return getColorAtIndex(buffer, x + (y * buffer.width));
}

// Arguments that can be configured for creating an image
struct Args {
  size_t height;
  size_t width;
  unsigned int minDensity;
  unsigned int maxDensity;
  unsigned int minRadius;
  unsigned int maxRadius;
  unsigned int strokeWidth;
  struct Color darkColor;
  struct Color lightColor;
  uint64_t seed;
  char *outfile;
  uint8_t verbose;
  uint8_t toStdout;
};

// Creates and returns an Args struct of default arguments
struct Args createDefaultArgs() {
  return (struct Args){
    .height = DEFAULT_HEIGHT,
    .width = DEFAULT_WIDTH,
    .minDensity = DEFAULT_MIN_DENSITY,
    .maxDensity = DEFAULT_MAX_DENSITY,
    .minRadius = DEFAULT_MIN_RADIUS,
    .maxRadius = DEFAULT_MAX_RADIUS,
    .darkColor = DEFAULT_BG_COLOR,
    .lightColor = DEFAULT_FG_COLOR,
    .seed = (uint64_t)time(NULL),
    .strokeWidth = DEFAULT_STROKE_WIDTH,
    .outfile = DEFAULT_FILE_NAME,
    .verbose = 0,
    .toStdout = 0
  };
}

// Flags:
// h: height
// w: width
// d: min density
// D: max density
// r: min radius
// R: max radius
// s: seed
// S: stroke width
// c: dark color
// C: light color
// o: outfile
// v: verbose
// O: print to stdout

// Parses arguments passed to program to Args struct, uses defaults for
// non-passed options.
struct Args parseCLIArgs(int argc, char **argv) {
  struct Args args = createDefaultArgs();

  int c;

  while ((c = getopt(argc, argv, "h:w:d:D:s:S:r:R:c:C:o:vO")) != -1) {
    switch (c) {
      case 'h':
        args.height = atoi(optarg);
        break;
      case 'w':
        args.width = atoi(optarg);
        break;
      case 'd':
        args.minDensity = atoi(optarg);
        break;
      case 'D':
        args.maxDensity = atoi(optarg);
        break;
      case 's':
        args.seed = atoi(optarg);
        break;
      case 'S':
        args.strokeWidth = atoi(optarg);
        break;
      case 'r':
        args.minRadius = atoi(optarg);
        break;
      case 'R':
        args.maxRadius = atoi(optarg);
        break;
      case 'c': {
        uint32_t _temp = strtol(optarg, NULL, 16);
        args.darkColor.red = (uint8_t)(_temp >> 16);
        args.darkColor.green = (uint8_t)(_temp >> 8);
        args.darkColor.blue = (uint8_t)_temp;
        };
        break;
      case 'C': {
        uint32_t _temp = strtol(optarg, NULL, 16);
        args.lightColor.red = (uint8_t)(_temp >> 16);
        args.lightColor.green = (uint8_t)(_temp >> 8);
        args.lightColor.blue = (uint8_t)_temp;
        };
        break;
      case 'o':
        free(args.outfile);
        args.outfile = strdup(optarg);
        break;
      case 'v':
        args.verbose = 1;
        break;
      case 'O':
        args.toStdout = 1;
        break;
    }
  }

  return args;
}

// Holds position and size of a Circle
struct Circle {
  size_t x;
  size_t y;
  unsigned int radius;
};

// Holds an array of circles and how many there are
struct CircleBuffer {
  size_t length;
  struct Circle *firstCircle;
};

// Using RNG, generates a number of circles
// within the bounds of density and radius
struct CircleBuffer generateCircles(struct Args args) {
  struct CircleBuffer cb = (struct CircleBuffer){ 0 };

  cb.length = (rand() % (args.maxDensity + 1 - args.minDensity)) + args.minDensity;

  struct Circle *buffer = (struct Circle*)malloc(sizeof(struct Circle) * cb.length);

  for (int i = 0; i < cb.length; i++) {
    buffer[i].x = rand() % args.width;
    buffer[i].y = rand() % args.height;
    buffer[i].radius = (rand() % (args.maxRadius + 1 - args.minRadius)) + args.minRadius;
    if (buffer[i].radius % 2 == 0) {
      /*if (buffer[i].radius + 1 > args.maxRadius) {
        buffer[i].radius -= 1;
      } else {
        buffer[i].radius += 1;
      }*/
      if (buffer[i].radius + 1 > args.maxRadius) {
        buffer[i].radius -= 1;
      } else if (buffer[i].radius - 1 < args.minRadius) {
        buffer[i].radius += 1;
      } else {
        buffer[i].radius += i % 2 ? 1 : -1;
      }
    }
  }

  cb.firstCircle = buffer;

  return cb;
}

// Compares circles for organizing with the smallest first
int compareCircles(const void *A, const void *B) {
  struct Circle cA = *(struct Circle*)A;
  struct Circle cB = *(struct Circle*)B;

  if (cA.radius > cB.radius) {
    return 1;
  } else if (cA.radius < cB.radius) {
    return -1;
  } else {
    return 0;
  }
}

// Sorts circles by smallest radius
void sortCircles(struct CircleBuffer *cb) {
  qsort(cb->firstCircle, cb->length, sizeof(struct Circle), compareCircles);
}

// Computes distances from points, used for distance from a given
// pixel to the centerpoint of a circle
float distance(size_t x1, size_t y1, size_t x2, size_t y2) {
  return sqrt(pow((int)(x2 - x1), 2) + pow((int)(y2 - y1), 2));
}

// Tests whether a circle is within the radius of a circle
unsigned char inCircleRadius(size_t x, size_t y, struct Circle circle, unsigned int strokeWidth) {
  return distance(x, y, circle.x, circle.y) <= circle.radius * strokeWidth;
}

// Runs for every pixel, checking its location relative to the circles and
// Returning a color based on the result of the check.
struct Color pixelFunction(size_t x, size_t y, struct Args args, struct CircleBuffer circles) {
  // Check if we are in a circle and if so, do the math within the loop and break out of it
  for (int i = 0; i < circles.length; i++) {
    if (inCircleRadius(x, y, circles.firstCircle[i], args.strokeWidth)) {
      float dist = distance(x, y, circles.firstCircle[i].x, circles.firstCircle[i].y);
      char oddOrEvenBand = ((int)(dist / args.strokeWidth)) % 2;

      if (oddOrEvenBand) {
        // Odd
        return args.lightColor;
      } else {
        // Even
        return args.darkColor;
      }
      
    }
  }

  // Otherwise, do the math for columns here

  char oddOrEvenColumn = (x / args.strokeWidth) % 2;
  if (oddOrEvenColumn) {
    // Odd
    return args.lightColor;
  } else {
    return args.darkColor;
  }
}

// Writes out PPM information to a file descriptor
void writePPMToDescriptor(FILE *fd, struct Args args, struct ColorBuffer buffer) {
  fprintf(fd, "P6\n");
  fprintf(fd, "%lu %lu\n", args.width, args.height);
  fprintf(fd, "255\n");
  fwrite(buffer.pixels, sizeof(struct Color), buffer.height * buffer.width, fd);
}

// Returns stdout if -O is passed to program, otherwise opens a file
FILE *getDescriptor(struct Args args) {
  if (args.toStdout) {
    return stdout;
  } else {
    return fopen(args.outfile, "wb");
  }
}

#ifndef JOMON_LIBRARY

int main(int argc, char *argv[]) {
  struct Args args = parseCLIArgs(argc, argv);

  if (args.verbose) {
    printf("Dimensions: %zux%zu\n", args.height, args.width);
    printf("Density range: %i-%i\n", args.minDensity, args.maxDensity);
    printf("seed: %" PRId64 "\n", args.seed);
    printf("strokeWidth: %i\n", args.strokeWidth);
    printf("Dark color: 0x%02X%02X%02X\n", args.darkColor.red, args.darkColor.green, args.darkColor.blue);
    printf("Light color: 0x%02X%02X%02X\n", args.lightColor.red, args.lightColor.green, args.lightColor.blue);
    printf("outfile: %s\n", args.outfile);
  }

  srand(args.seed);

  struct CircleBuffer circleBuffer = generateCircles(args);
  sortCircles(&circleBuffer);

  if (args.verbose) {
    for (int i = 0; i < circleBuffer.length; i++) {
      printf("Circle #%i: (%zu, %zu) and radius %i\n", i, circleBuffer.firstCircle[i].x, circleBuffer.firstCircle[i].y, circleBuffer.firstCircle[i].radius);
    }
  }

  struct ColorBuffer imageBuffer = createImageBuffer(args.width, args.height);

  for (int x = 0; x < args.width; x++) {
    for (int y = 0; y < args.height; y++) {
      struct Color pixColor = pixelFunction(x, y, args, circleBuffer);
      struct Color *toPaint = getColorAtCoordinates(imageBuffer, x, y);

      toPaint->red = pixColor.red;
      toPaint->green = pixColor.green;
      toPaint->blue = pixColor.blue;
    }
  }

  FILE *file = getDescriptor(args);
  if (file == NULL) exit(1);

  writePPMToDescriptor(file, args, imageBuffer);
  
  fclose(file);

  return 0;
}

#endif