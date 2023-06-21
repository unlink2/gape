#include "libgape/config.h"
#include "libgape/buffer.h"
#include <stdlib.h>
#include <string.h>

struct gape_config gape_config_init(void) {
  struct gape_config c;
  memset(&c, 0, sizeof(c));
  c.exec_args = gape_buffer_init();
  return c;
}

void gape_config_exec(struct gape_config *self, const char *prg,
                      const char **args, size_t args_len) {
  // combine all args into a single string using the buffer struct
}

void gape_config_free(struct gape_config *self) {
  if (self->prg_args != NULL) {
    free(self->prg_args);
  }

  gape_buffer_free(&self->exec_args);
}
