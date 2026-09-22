#include <cs50.h>

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "TinyPngOut.h"
#include "qrcodegen.h"

#define SCALE 10
#define MAX_MASK_NUMBER 7
#define MIN_MASK_NUMBER 0

typedef enum {
  MASK_0 = 0,
  MASK_1,
  MASK_2,
  MASK_3,
  MASK_4,
  MASK_5,
  MASK_6,
  MASK_7
} Mask;

int get_mask_number(void) {
  int mask_number;
  do {
    mask_number = get_int("Enter your MaskNumber (0-7): ");
    if (mask_number == INT_MAX) {
      fprintf(stderr, "No input provided.\n");
      exit(EXIT_FAILURE);
    }
    if (mask_number < MIN_MASK_NUMBER || mask_number > MAX_MASK_NUMBER) {
      printf("Invalid mask number. Please enter a number between 0 and 7.\n");
    }
  } while (mask_number < MIN_MASK_NUMBER || mask_number > MAX_MASK_NUMBER);
  return mask_number;
}

Mask convert_int_to_mask(int mask_number) { return (Mask)mask_number; }

bool create_qrcode_image(struct TinyPngOut *writer, uint8_t *qr, int size) {
  if (writer == NULL || qr == NULL) {
    fprintf(stderr, "Error: Invalid parameters\n");
    return false;
  }

  uint8_t black_color[3] = {0, 0, 0};
  uint8_t white_color[3] = {255, 255, 255};

  for (int y = 0; y < size * SCALE; y++) {
    for (int x = 0; x < size * SCALE; x++) {
      bool color = qrcodegen_getModule(qr, x / SCALE, y / SCALE);
      uint8_t *pixel_color = color ? black_color : white_color;
      if (TinyPngOut_write(writer, pixel_color, 1) != TINYPNGOUT_OK) {
        fprintf(stderr, "Error writing PNG data\n");
        return false;
      }
    }
  }

  return true;
}

FILE *create_output_file(const char *filepath) {
  if (filepath == NULL) {
    fprintf(stderr, "Error: NULL filepath\n");
    return NULL;
  }

  FILE *filepointer = fopen(filepath, "wb");
  if (filepointer == NULL) {
    perror("Error creating file");
  }
  return filepointer;
}

bool init_tinypng_output(struct TinyPngOut *writer, int width, int height,
                         FILE *filepointer) {
  if (writer == NULL || filepointer == NULL) {
    fprintf(stderr, "Error: Invalid parameters\n");
    return false;
  }

  if (TinyPngOut_init(writer, width, height, filepointer) != TINYPNGOUT_OK) {
    fprintf(stderr, "Error initializing TinyPngOut\n");
    return false;
  }
  return true;
}

int main(void) {
  const char *filepath = "./output.png";
  FILE *filepointer = create_output_file(filepath);

  if (filepointer == NULL) {
    return EXIT_FAILURE;
  }

  int mask_number = get_mask_number();
  Mask mask_enum = convert_int_to_mask(mask_number);
  enum qrcodegen_Mask mask_version = (enum qrcodegen_Mask)mask_enum;

  uint8_t temp_buffer[qrcodegen_BUFFER_LEN_MAX];
  uint8_t qr_code[qrcodegen_BUFFER_LEN_MAX];

  char *qr_code_text = get_string("Enter your QR code text: ");
  if (qr_code_text == NULL) {
    fprintf(stderr, "Error reading input\n");
    fclose(filepointer);
    return EXIT_FAILURE;
  }

  bool success = qrcodegen_encodeText(
      qr_code_text, temp_buffer, qr_code, qrcodegen_Ecc_MEDIUM,
      qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, mask_version, true);

  if (!success) {
    fprintf(stderr, "Error generating QR code\n");
    fclose(filepointer);
    return EXIT_FAILURE;
  }

  int size = qrcodegen_getSize(qr_code);
  printf("QR code size: %d\n", size);

  struct TinyPngOut writer;
  if (!init_tinypng_output(&writer, size * SCALE, size * SCALE, filepointer)) {
    fclose(filepointer);
    return EXIT_FAILURE;
  }

  if (!create_qrcode_image(&writer, qr_code, size)) {
    fprintf(stderr, "Error creating QR code image\n");
    fclose(filepointer);
    return EXIT_FAILURE;
  }

  fclose(filepointer);

  printf("QR code saved to %s\n", filepath);
  return EXIT_SUCCESS;
}
