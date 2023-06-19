#include <stdio.h>
#include <stdlib.h>
#include "libgape/config.h"
#include "libgape/log.h"
#include "arg.h"
#include "libgape/watch.h"

int main(int argc, char **argv) {
  struct gape_config cfg = gape_args_to_config(argc, argv);

  struct gape_watch watch = gape_watch_from_cfg(&cfg);
  int ret = gape_watch(&watch);
  gape_watch_free(&watch);

  gape_config_free(&cfg);
  return ret;
}
