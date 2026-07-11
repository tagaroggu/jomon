#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

struct ColorBuffer {
  size_t width;
  size_t height;
  struct Color pixels[];
};

struct ColorBuffer *createImageBuffer(size_t width, size_t height) {
  struct ColorBuffer *output = (struct ColorBuffer *)malloc(
      sizeof(struct ColorBuffer) + (sizeof(struct Color) * width * height) -
      sizeof(struct Color));
  output->width = width;
  output->height = height;

  return output;
}

struct Color *getColorAtIndex(struct ColorBuffer *buffer, size_t index) {
  if (index >= buffer->width * buffer->height)
    return NULL;
  return &buffer->pixels[index];
}

struct Color *getColorAtCoordinates(struct ColorBuffer *buffer, size_t x,
                                    size_t y) {
  if (x < 0 || y < 0 || x >= buffer->width || y >= buffer->height)
    return NULL;
  return getColorAtIndex(buffer, x + (y * buffer->width));
}

// TODO: Implment
char *convertBufferToPPM(struct ColorBuffer *buffer) {
  exit(1);

  return NULL;
}

int main(int argc, char *argv[]) {
  exit(1);

  return 0;
}
