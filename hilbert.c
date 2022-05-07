//image writing 
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include "stb_image_write.h"

#define EXPECTED_ARGC 3
#define HELP_ARGC 2
#define FILE_ARGI 2
#define OPTN_ARGI 1
//
#define LAST_2_BIT 3
#define SECOND_BIT 2
#define FIRST_BIT 1
//quadrants
#define BOTTOM_LEFT 0
#define TOP_LEFT 1
#define TOP_RIGHT 2
#define BOTTOM_RIGHT 3
//
#define BITS_IN_A_BYTE 8
#define C 3
#define B 1

typedef struct image{
  uint8_t channels;
  double bytesPerPixel;
  uint8_t bitsPerPixel;
  uint32_t maxWidth, maxHeight;
  uint8_t *pixels;
  uint64_t maxLength;
  void (*writeStrategy)(FILE *, struct image *, uint64_t, uint64_t *);
} image;

typedef struct curve{
  uint32_t x, y;
  uint32_t height; //the square width/height of the curve
} curve;

void throw(char* e){
  fprintf(stderr, "%s", e);
  fflush(stderr);
  exit(1);
}

uint8_t lastTwoBits(uint64_t n){
  return n & LAST_2_BIT;
}

/*
algorithm based on this article: http://blog.marcinchwedczuk.pl/iterative-algorithm-for-drawing-hilbert-curve
n is the index along the hilbert curve we want
height is the side length of the cartesian space of the hilbert curve
*/
void hilToCart(uint64_t index, uint32_t height, uint32_t *x, uint32_t *y){ 
  if(sqrt(index) >= height) throw("Index passed to hilToCart unexpectedly large.");


  //compute positions of side length/height 2 curve
  uint8_t positions[4][2] = {
    {0, 0},
    {0, 1},
    {1, 1},
    {1, 0}
  };

  //encode the position of node in order 1 (height=2) curve
  *x = positions[lastTwoBits(index)][0];
  *y = positions[lastTwoBits(index)][1];
  index = index >> 2;

  //side length doubles as positions in the curve quadruple each step
  for(uint32_t n = 4; n <= height; n *= 2){ 
    uint32_t n2 = n / 2; //previously computed height 
    uint8_t quadrant = lastTwoBits(index);

    if(quadrant == BOTTOM_LEFT){
      //rotate 90 cw and flip top-bottom
      *y += *x;
      *x = *y - *x;
      *y -= *x;
    } 
    if(quadrant == TOP_LEFT) {
      //lift up by the previous height
      *y += n2;
    }
    if(quadrant == TOP_RIGHT) {
      //lift up and translate along by previous height
      *x += n2;
      *y += n2;
    }
    if(quadrant == BOTTOM_RIGHT) { 
      //rotate 90 ccw and translate along by previous height 
      uint32_t tmp = *y;
      *y = (n2 - 1) - *x;
      *x = (n2 - 1) - tmp;
      *x = *x + n2;
    }

    index = index >> 2;
  }
} 

FILE *fopenCheck(char *file, char *mode){
  FILE *p = fopen(file, mode);
  if(p != NULL) return p;
  fprintf(stderr, "Can't open %s", file);
  fflush(stderr);
  exit(1); 
}

uint64_t getFileSize(FILE *fp){
  uint64_t count = 0; 
  uint8_t byte = fgetc(fp);
  while(!feof(fp)){
    byte = fgetc(fp);
    count++; 
  }
  return count;
}

void getImageSize(uint64_t positions, uint32_t height, uint32_t *maxWidth, uint32_t *maxHeight){
  *maxWidth = 0; *maxHeight = 0;
  uint32_t x, y; 
  for(int i = 0; i < positions; i++){
    hilToCart(i, height, &x, &y);
    *maxWidth = x > *maxWidth ? x : *maxWidth;
    *maxHeight = y > *maxHeight ? y : *maxHeight;
  }
  *maxWidth = *maxWidth + 1;
  *maxHeight = *maxHeight + 1;
}

void uintDecrement(uint64_t *x){
  *x = *x == 0 ? *x : *x - 1;
}


void writeThreeBytes(FILE *in, image *img, uint64_t index, uint64_t *remainingBytes){
  for(uint8_t i = 0; i < 3; i++){
    if(index + i > img->maxLength * img->channels) { throw("Index out of range of pixels."); }
    img->pixels[index + i] = fgetc(in);
    uintDecrement(remainingBytes);
  }
}

void writeByte(FILE *in, image *img, uint64_t index, uint64_t *remainingBytes){
  if(index > img->maxLength) { throw("Index out of range of pixels."); }
  img->pixels[index] = fgetc(in);
  uintDecrement(remainingBytes);
}

void writeBit(FILE *in, image *img, uint64_t index, uint64_t *remainingBytes){
  //Implement me!
}


void arrange(FILE *in, image *img, curve *crv, uint64_t remainingBytes, uint64_t maxLength){
  fseek(in, 0, SEEK_SET); //reset file position
  uint8_t loadingBarUpdates = 50;
  crv->x = 0; crv->y = 0;
  for(uint64_t n = 0; n < maxLength; n++){
    if(n % ((maxLength / loadingBarUpdates) + 1) == 0) { 
      printf("%d%% complete...\r", (int) floor((double) (n * 100) / maxLength)); 
      fflush(stdout); 
    }
    hilToCart(n, crv->height, &crv->x, &crv->y);
    uint64_t index = (crv->x * img->channels) + (img->maxWidth * crv->y * img->channels);
    if(crv->x < img->maxWidth && crv->y < img->maxHeight){ 
      if(remainingBytes > 0){ 
        img->writeStrategy(in, img, index, &remainingBytes); 
      } else {
        for(uint8_t i = 0; i < img->channels; i++){//write blanks
          img->pixels[index + i] = 0x00;
        }
      }
    }
  }
}

int main(int argc, char** args){

  if(argc == HELP_ARGC && strcmp(args[OPTN_ARGI], "--help") == 0){
    printf("Usage: ./hilbert [OPTN] [FILE]\n   -b -> shows file in bytes.\n   -c -> shows file in colour (triplets of bytes).\nThe FILE argument is the path of any file stored as bytes."); exit(0);
  }
  if(argc != EXPECTED_ARGC) { 
    throw("Incorrect number of arguments, try './hilbert --help' for more information.");
  }

  char *optn = args[OPTN_ARGI];
  char *file = args[FILE_ARGI];
  curve crv;
  image img;

  FILE *in = fopenCheck(file, "rb");
  uint8_t bitsPerPixel = strcmp(optn, "-b") == 0 ? 8 :
                         strcmp(optn, "-c") == 0 ? 24:
                         0;
  if(bitsPerPixel == 0) { throw("Invalid option, try './hilbert --help' for more information."); }
  double bytesPerPixel = (double) bitsPerPixel / BITS_IN_A_BYTE; //0.125 if 1b per pixel, 1 if 1B, 3 if 3B
  uint8_t channels = (int) ceil(bytesPerPixel);

  uint64_t fileSizeBytes = getFileSize(in);
  uint64_t positions = ceil((double) fileSizeBytes / bytesPerPixel); //pixels in the image with file data
  uint32_t order = floorl((double) log((double) (positions - 1)) / log(4)) + 1;
  uint32_t height = (uint32_t) sqrt(pow(4, order));
  uint32_t maxWidth, maxHeight; getImageSize(positions, height, &maxWidth, &maxHeight);
  printf("Your file will render to an order %d hilbert curve containing %lu pixels.\nImage size: %dx%d.\nEach pixel will represent %d bit%s\n", order, positions, maxWidth, maxHeight, bitsPerPixel, bitsPerPixel > 1 ? "s." : ".");

  //needs channels bytes for each pixel (1 in the case of -b or -B, 3 in the case of -C)

  uint64_t maxLength = (uint64_t) pow(4, order);
  uint8_t *pixels = malloc(maxLength * channels);
  //printf("length %lu\n", length);
  void (*writeStrategy)(FILE *, image *, uint64_t, uint64_t *) 
                      = bitsPerPixel == 1 ? (writeBit) :
                        bitsPerPixel == 8 ? (writeByte) :
                        (writeThreeBytes); 

  crv.height = height;
  img.maxLength = maxLength;//points in the hilbert curve
  img.bitsPerPixel = bitsPerPixel;
  img.bytesPerPixel = bytesPerPixel;
  img.writeStrategy = writeStrategy;
  img.channels = channels;
  img.pixels = pixels;
  img.maxWidth = maxWidth; img.maxHeight = maxHeight;
  arrange(in, &img, &crv, fileSizeBytes, maxLength); 

  printf("Writing image...                               \r"); fflush(stdout);
  if(stbi_write_png("hilbert.png", maxWidth, maxHeight, channels, pixels, maxWidth * channels) == 0) { throw("stb failed to write image."); }
  printf("Complete.                                      \n");

  free(pixels);
  fclose(in);

  return 0;
}
