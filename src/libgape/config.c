#include "libgape/config.h"
#include <stdlib.h>
#include <string.h>

struct gape_config gape_config_init(void) {
  struct gape_config c;
  memset(&c, 0, sizeof(c));

  return c;
}

void gape_config_free(struct gape_config *self) {
  if (self->prg_args != NULL) {
    free(self->prg_args);
  }
}
