#include "libgape/vec.h"
#include "libgape/error.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct gape_vec gape_vec_init(size_t stride) {
  struct gape_vec self;
  memset(&self, 0, sizeof(self));

  self.data = malloc(GAPE_VEC_INITIAL_LEN * stride);
  self.stride = stride;
  self.len_max = GAPE_VEC_INITIAL_LEN;
  memset(self.data, 0, self.len_max);

  return self;
}

#define gape_vec_data_at(self, index)                                          \
  ((uint8_t *)((self)->data) + (self)->stride * (index))

void *gape_vec_get(struct gape_vec *self, size_t index) {
  if (index > self->len) {
    return NULL;
  }

  return gape_vec_data_at(self, index);
}

void gape_vec_resize(struct gape_vec *self) {
  self->len_max *= 2;
  void *new_data = realloc(self->data, self->len_max * self->stride);
  if (!new_data) {
    gape_errno();
    return;
  }

  self->data = new_data;
}

void gape_vec_add(struct gape_vec *self, void *data) {
  if (self->len >= self->len_max) {
    gape_vec_resize(self);
    if (gape_err()) {
      return;
    }
  }
  memcpy(gape_vec_data_at(self, self->len), data, self->stride);
  self->len++;
}

void gape_vec_free(struct gape_vec *self) { free(self->data); }
