#include "libgape/watch.h"
#include <stdlib.h>
#include <string.h>

struct GapeWatch gape_watch_init(void) {
  struct GapeWatch self;
  memset(&self, 0, sizeof(self));

  return self;
}

struct GapeWatch gape_watch_time_init(uint64_t seconds) {
  struct GapeWatch self = gape_watch_init();

  return self;
}

void gape_watch_free(struct GapeWatch *self) {
  if (self->watch_cfg) {
    free(self->watch_cfg);
  }

  if (self->watch_var) {
    free(self->watch_var);
  }
}
