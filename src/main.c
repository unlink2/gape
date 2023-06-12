#include <stdio.h>
#include <stdlib.h>
#include "libgape/config.h"
#include "libgape/log.h"
#include "arg.h"

int main(int argc, char **argv) {
  struct GapeConfig cfg = gape_args_to_config(argc, argv);
  return 0;
}
