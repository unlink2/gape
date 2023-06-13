#ifndef BUFFER_H_
#define BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#define GAPE_BUFFER_INITIAL_LEN 128

/**
 * Simple expanding string buffer
 */
struct gape_buffer {
  uint8_t *data;
  size_t index;
  size_t max_len;
};

struct gape_buffer gape_buffer_init(void);

// reserve the next 'len' bytes
// for use - if the buffer does not have enough data at this point
// it will resize to fit twice the requested length
// and return the apropriate pointer
uint8_t *gape_buffer_next(struct gape_buffer *self, size_t len);

// advance index by n bytes
void gape_buffer_adv(struct gape_buffer *self, size_t n);

void gape_buffer_free(struct gape_buffer *self);

#endif
