#include <stdio.h>
#include <stdlib.h>
#include "libgape/config.h"
#include "libgape/log.h"
#include "arg.h"
#include "libgape/watch.h"

int main(int argc, char **argv) {
  struct gape_config cfg = gape_args_to_config(argc, argv);

  const char **e = cfg.prg_args;
  while (*e) {
    printf("%s\n", *e);
    e++;
  }

  gape_config_free(&cfg);
  return 0;
}
