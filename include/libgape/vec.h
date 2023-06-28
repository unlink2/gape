#ifndef VEC_H_
#define VEC_H_

#include <stddef.h>

#define GAPE_VEC_INITIAL_LEN 16

struct gape_vec {
  void *data;
  size_t len;
  size_t len_max;
  size_t stride;
};

struct gape_vec gape_vec_init(size_t stride);

void *gape_vec_get(struct gape_vec *self, size_t index);
void gape_vec_add(struct gape_vec *self, void *data);

void gape_vec_free(struct gape_vec *self);

#endif
