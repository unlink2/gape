#include "libgape/buffer.h"
#include "libgape/error.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GapeBuffer gape_buffer_init(void) {
  struct GapeBuffer self;
  memset(&self, 0, sizeof(self));

  self.data = malloc(GAPE_BUFFER_INITIAL_LEN);
  self.max_len = GAPE_BUFFER_INITIAL_LEN;

  return self;
}

void gape_buffer_resize(struct GapeBuffer *self, size_t by) {
  size_t new_max_len = self->max_len + by;
  uint8_t *new_data = realloc(self->data, self->max_len);

  if (new_data) {
    self->max_len = new_max_len;
    self->data = new_data;
  } else {
    gape_err_set(GAPE_ERR_REALLOC);
  }
}

uint8_t *gape_buffer_next(struct GapeBuffer *self, size_t len) {
  if (self->index + len >= self->max_len) {
    gape_buffer_resize(self, len * 2);
    if (gape_err()) {
      return NULL;
    }
  }

  self->index += len;
  return self->data + self->index;
}

void gape_buffer_free(struct GapeBuffer *self) { free(self->data); }
