/* TODO: Implementation of the whole thing. Write a function that is run for
 * each pixel that checks whether its within the area of a circle and if so its
 * distance to the center and therefore the pixel's color, otherwise which
 * column it is in and which color to match the column that it is in.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 1920
#endif

#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 1080
#endif

#ifndef DEFAULT_STROKE_WIDTH
#define DEFAULT_STROKE_WIDTH 5
#endif

#ifndef DEFAULT_BG_COLOR
#define DEFAULT_BG_COLOR (struct Color){0, 0, 0}
#endif

#ifndef DEFAULT_FG_COLOR
#define DEFAULT_FG_COLOR (struct Color){255, 255, 255}
#endif

#ifndef DEFAULT_MIN_DENSITY
#define DEFAULT_MIN_DENSITY 5
#endif

#ifndef DEFAULT_MAX_DENSITY
#define DEFAULT_MAX_DENSITY 20
#endif

#ifndef DEFAULT_SEED
#define DEFAULT_SEED time(NULL)
#endif

#ifndef DEFAULT_FILE_NAME
#define DEFAULT_FILE_NAME ({ char timeStr[256]; \
  snprintf(timeStr, 256, "%lu", time(NULL)); \
  size_t strLen = strlen(timeStr) +  13; char *_temp = (char*)malloc(sizeof(char) * strLen); \
  snprintf(_temp, strLen, "./jomon-%s.ppm", timeStr); _temp; })
#endif

#ifndef DEFAULT_MIN_RADIUS
#define DEFAULT_MIN_RADIUS 3
#endif

#ifndef DEFAULT_MAX_RADIUS
#define DEFAULT_MAX_RADIUS 12
#endif

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

struct ColorBuffer {
  size_t width;
  size_t height;
  struct Color *pixels;
};

struct ColorBuffer createImageBuffer(size_t width, size_t height) {
  return (struct ColorBuffer){
    .width = width,
    .height = height,
    .pixels = (struct Color*)malloc(sizeof(struct Color) * width * height)
  };
}

struct Color *getColorAtIndex(struct ColorBuffer *buffer, size_t index) {
  if (index >= buffer->width * buffer->height)
    return NULL;
  return &buffer->pixels[index];
}

struct Color *getColorAtCoordinates(struct ColorBuffer *buffer, size_t x, size_t y) {
  if (x < 0 || y < 0 || x >= buffer->width || y >= buffer->height)
    return NULL;
  return getColorAtIndex(buffer, x + (y * buffer->width));
}

// TODO: Implment
char *convertBufferToPPM(struct ColorBuffer *buffer) {
  exit(1);

  return NULL;
}

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
};

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
    .verbose = 0
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
struct Args parseCLIArgs(int argc, char **argv) {
  struct Args args = createDefaultArgs();

  int c;

  while ((c = getopt(argc, argv, "h:w:d:D:s:S:c:C:o:v")) != -1) {
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
    }
  }

  return args;
}

struct Circle {
  size_t x;
  size_t y;
  unsigned int radius;
};

struct CircleBuffer {
  size_t length;
  struct Circle *firstCircle;
};

struct CircleBuffer generateCircles(struct Args args) {
  struct CircleBuffer cb = (struct CircleBuffer){ 0 };

  cb.length = (rand() % (args.maxDensity + 1 - args.minDensity)) + args.minDensity;

  struct Circle *buffer = (struct Circle*)malloc(sizeof(struct Circle) * cb.length);

  for (int i = 0; i < cb.length; i++) {
    buffer[i].x = rand() % args.width;
    buffer[i].y = rand() % args.height;
    buffer[i].radius = (rand() % (args.maxRadius + 1 - args.minRadius)) + args.minRadius;
  }

  cb.firstCircle = buffer;

  return cb;
}

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

void sortCircles(struct CircleBuffer *cb) {
  qsort(cb->firstCircle, cb->length, sizeof(struct Circle), compareCircles);
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

  exit(1);
  return 0;
}

#endif