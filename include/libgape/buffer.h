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

void gape_buffer_clear(struct gape_buffer *self);

// returns the start address of the current buffer ptr
const uint8_t *gape_buffer_start(struct gape_buffer *self);
size_t gape_buffer_len(struct gape_buffer *self);

// this call NULL terminates the buffer by appending \0
// This will *not* advance the buffer. it simply ensures
// termination
void gape_buffer_null_term(struct gape_buffer *self);

void gape_buffer_free(struct gape_buffer *self);

#endif
