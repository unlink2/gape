#include "libgape/config.h"
#include <string.h>

struct gape_config gape_config_init(void) {
  struct gape_config c;
  memset(&c, 0, sizeof(c));

  return c;
}
