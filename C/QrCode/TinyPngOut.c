/*
 * Tiny PNG Output (C)
 *
 * Copyright (c) 2018 Project Nayuki
 * https://www.nayuki.io/page/tiny-png-output
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program (see COPYING.txt and COPYING.LESSER.txt).
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "TinyPngOut.h"

static const uint16_t DEFLATE_MAX_BLOCK_SIZE = 65535;


static bool write(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                  size_t len);
static void crc32(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                  size_t len);
static void adler32(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                    size_t len);
static void putBigUint32(uint32_t val, TPO_FIX_ARR(uint8_t array));

enum TinyPngOut_Status TinyPngOut_init(TPO_DYN_ARR(struct TinyPngOut self),
                                       uint32_t w, uint32_t h,
                                       TPO_DYN_ARR(FILE out)) {
  // Check arguments
  if (w == 0 || h == 0 || out == NULL)
    return TINYPNGOUT_INVALID_ARGUMENT;
  self->width = w;
  self->height = h;

  // Compute and check data siezs
  uint64_t lineSz = (uint64_t)self->width * 3 + 1;
  if (lineSz > UINT32_MAX)
    return TINYPNGOUT_IMAGE_TOO_LARGE;
  self->lineSize = (uint32_t)lineSz;

  uint64_t uncompRm = self->lineSize * self->height;
  if (uncompRm > UINT32_MAX)
    return TINYPNGOUT_IMAGE_TOO_LARGE;
  self->uncompRemain = (uint32_t)uncompRm;

  uint32_t numBlocks = self->uncompRemain / DEFLATE_MAX_BLOCK_SIZE;
  if (self->uncompRemain % DEFLATE_MAX_BLOCK_SIZE != 0)
    numBlocks++; // Round up
  // 5 bytes per DEFLATE uncompressed block header, 2 bytes for zlib header, 4
  // bytes for zlib Adler-32 footer
  uint64_t idatSize = (uint64_t)numBlocks * 5 + 6;
  idatSize += self->uncompRemain;
  if (idatSize > (uint32_t)INT32_MAX)
    return TINYPNGOUT_IMAGE_TOO_LARGE;

  // Write header (not a pure header, but a couple of things concatenated
  // together)
  uint8_t header[] = {
      // 43 bytes long
      // PNG header
      0x89,
      0x50,
      0x4E,
      0x47,
      0x0D,
      0x0A,
      0x1A,
      0x0A,
      // IHDR chunk
      0x00,
      0x00,
      0x00,
      0x0D,
      0x49,
      0x48,
      0x44,
      0x52,
      0,
      0,
      0,
      0, // 'width' placeholder
      0,
      0,
      0,
      0, // 'height' placeholder
      0x08,
      0x02,
      0x00,
      0x00,
      0x00,
      0,
      0,
      0,
      0, // IHDR CRC-32 placeholder
      // IDAT chunk
      0,
      0,
      0,
      0, // 'idatSize' placeholder
      0x49,
      0x44,
      0x41,
      0x54,
      // DEFLATE data
      0x08,
      0x1D,
  };
  putBigUint32(self->width, &header[16]);
  putBigUint32(self->height, &header[20]);
  putBigUint32(idatSize, &header[33]);
  self->crc = 0;
  crc32(self, &header[12], 17);
  putBigUint32(self->crc, &header[29]);
  self->output = out;
  if (!write(self, header, sizeof(header) / sizeof(header[0])))
    return TINYPNGOUT_IO_ERROR;

  self->crc = 0;
  crc32(self, &header[37], 6); // 0xD7245B6B
  self->adler = 1;

  self->positionX = 0;
  self->positionY = 0;
  self->deflateFilled = 0;
  return TINYPNGOUT_OK;
}

enum TinyPngOut_Status TinyPngOut_write(TPO_DYN_ARR(struct TinyPngOut self),
                                        const uint8_t pixels[], size_t count) {
  if (count > SIZE_MAX / 3)
    return TINYPNGOUT_INVALID_ARGUMENT;
  count *= 3; // Convert pixel count to byte count
  while (count > 0) {
    if (pixels == NULL)
      return TINYPNGOUT_INVALID_ARGUMENT;
    if (self->positionY >= self->height)
      return TINYPNGOUT_INVALID_ARGUMENT; // All image pixels already written

    if (self->deflateFilled == 0) { // Start DEFLATE block
      uint16_t size = DEFLATE_MAX_BLOCK_SIZE;
      if (self->uncompRemain < size)
        size = (uint16_t)self->uncompRemain;
      const uint8_t header[] = {
          // 5 bytes long
          (uint8_t)(self->uncompRemain <= DEFLATE_MAX_BLOCK_SIZE ? 1 : 0),
          (uint8_t)(size >> 0),
          (uint8_t)(size >> 8),
          (uint8_t)((size >> 0) ^ 0xFF),
          (uint8_t)((size >> 8) ^ 0xFF),
      };
      if (!write(self, header, sizeof(header) / sizeof(header[0])))
        return TINYPNGOUT_IO_ERROR;
      crc32(self, header, sizeof(header) / sizeof(header[0]));
    }
    assert(self->positionX < self->lineSize &&
           self->deflateFilled < DEFLATE_MAX_BLOCK_SIZE);

    if (self->positionX == 0) { // Beginning of line - write filter method byte
      uint8_t b[] = {0};
      if (!write(self, b, sizeof(b) / sizeof(b[0])))
        return TINYPNGOUT_IO_ERROR;
      crc32(self, b, 1);
      adler32(self, b, 1);
      self->positionX++;
      self->uncompRemain--;
      self->deflateFilled++;

    } else { // Write some pixel bytes for current line
      uint16_t n = DEFLATE_MAX_BLOCK_SIZE - self->deflateFilled;
      if (self->lineSize - self->positionX < n)
        n = (uint16_t)(self->lineSize - self->positionX);
      if (count < n)
        n = (uint16_t)count;
      assert(n > 0);
      if (!write(self, pixels, n))
        return TINYPNGOUT_IO_ERROR;

      // Update checksums
      crc32(self, pixels, n);
      adler32(self, pixels, n);

      // Increment positions
      count -= n;
      pixels += n;
      self->positionX += n;
      self->uncompRemain -= n;
      self->deflateFilled += n;
    }

    if (self->deflateFilled >= DEFLATE_MAX_BLOCK_SIZE)
      self->deflateFilled = 0; // End current block

    if (self->positionX == self->lineSize) { // Increment line
      self->positionX = 0;
      self->positionY++;
      if (self->positionY == self->height) { // Reached end of pixels
        uint8_t footer[] = {
            // 20 bytes long
            0,
            0,
            0,
            0, // DEFLATE Adler-32 placeholder
            0,
            0,
            0,
            0, // IDAT CRC-32 placeholder
            // IEND chunk
            0x00,
            0x00,
            0x00,
            0x00,
            0x49,
            0x45,
            0x4E,
            0x44,
            0xAE,
            0x42,
            0x60,
            0x82,
        };
        putBigUint32(self->adler, &footer[0]);
        crc32(self, &footer[0], 4);
        putBigUint32(self->crc, &footer[4]);
        if (!write(self, footer, sizeof(footer) / sizeof(footer[0])))
          return TINYPNGOUT_IO_ERROR;
      }
    }
  }
  return TINYPNGOUT_OK;
}

/*---- Private utility functions ----*/

// Returns whether the write was successful.
static bool write(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                  size_t len) {
  return fwrite(data, sizeof(data[0]), len, self->output) == len;
}

// Reads the 'crc' field and updates its value based on the given array of new
// data.
static void crc32(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                  size_t len) {
  self->crc = ~self->crc;
  for (size_t i = 0; i < len; i++) {
    for (int j = 0; j < 8;
         j++) { // Inefficient bitwise implementation, instead of table-based
      uint32_t bit = (self->crc ^ (data[i] >> j)) & 1;
      self->crc = (self->crc >> 1) ^ ((-bit) & UINT32_C(0xEDB88320));
    }
  }
  self->crc = ~self->crc;
}

// Reads the 'adler' field and updates its value based on the given array of new
// data.
static void adler32(TPO_DYN_ARR(struct TinyPngOut self), const uint8_t data[],
                    size_t len) {
  uint32_t s1 = self->adler & 0xFFFF;
  uint32_t s2 = self->adler >> 16;
  for (size_t i = 0; i < len; i++) {
    s1 = (s1 + data[i]) % 65521;
    s2 = (s2 + s1) % 65521;
  }
  self->adler = s2 << 16 | s1;
}

static void putBigUint32(uint32_t val, TPO_FIX_ARR(uint8_t array)) {
  for (int i = 0; i < 4; i++)
    array[i] = (uint8_t)(val >> ((3 - i) * 8));
}
