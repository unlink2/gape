#include "libgape/config.h"
#include "libgape/buffer.h"
#include "libgape/watch.h"
#include <stdlib.h>
#include <string.h>

struct gape_config gape_config_init(void) {
  struct gape_config c;
  memset(&c, 0, sizeof(c));

  c.usleep = GAPE_SPIN_MS;
  c.n_runs = GAPE_NRUN_FOREVER;

  return c;
}

void gape_config_free(struct gape_config *self) {
  if (self->prg_args) {
    free(self->prg_args);
  }

  if (self->_free_me_sh_c) {
    free(self->_free_me_sh_c);
  }
}
