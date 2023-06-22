#include "libgape/buffer.h"
#include "libgape/error.h"
#include "libgape/log.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct gape_buffer gape_buffer_init(void) {
  struct gape_buffer self;
  memset(&self, 0, sizeof(self));

  self.data = malloc(GAPE_BUFFER_INITIAL_LEN);
  memset(self.data, 0, GAPE_BUFFER_INITIAL_LEN);
  self.max_len = GAPE_BUFFER_INITIAL_LEN;

  return self;
}

void gape_buffer_resize(struct gape_buffer *self, size_t by) {
  size_t new_max_len = self->max_len + by;
  uint8_t *new_data = realloc(self->data, new_max_len);

  if (new_data) {
    gape_dbg(
        "Resized buffer %p:%ld by %ld bytes (to %ld). Old address: %p. New "
        "address: %p\n",
        (void *)self, self->max_len, by, new_max_len, (void *)self->data,
        (void *)new_data);
    self->max_len = new_max_len;
    self->data = new_data;
  } else {
    gape_err_set(GAPE_ERR_REALLOC);
  }
}

uint8_t *gape_buffer_next(struct gape_buffer *self, size_t len) {
  if (self->index + len >= self->max_len) {
    gape_buffer_resize(self, len * 2);
    if (gape_err()) {
      return NULL;
    }
  }

  return self->data + self->index;
}

void gape_buffer_adv(struct gape_buffer *self, size_t n) { self->index += n; }

void gape_buffer_clear(struct gape_buffer *self) { self->index = 0; }

const uint8_t *gape_buffer_start(struct gape_buffer *self) {
  gape_dbg_assert(self->data);
  return self->data;
}

size_t gape_buffer_len(struct gape_buffer *self) { return self->index; }

void gape_buffer_null_term(struct gape_buffer *self) {
  uint8_t *next = gape_buffer_next(self, 1);
  *next = '\0';
}

uint8_t *gape_buffer_move(struct gape_buffer *self) { return self->data; }

void gape_buffer_free(struct gape_buffer *self) { free(self->data); }
