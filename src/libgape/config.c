#include "libgape/config.h"
#include <string.h>

struct GapeConfig gape_config_init(void) {
  struct GapeConfig c;
  memset(&c, 0, sizeof(c));

  return c;
}
