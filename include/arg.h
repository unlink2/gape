#ifndef ARG_H_
#define ARG_H_

#include "argtable2.h"
#include <stdlib.h>

#include "libgape/config.h"
#include "libgape/log.h"
#include "libgape/macros.h"

struct GapeConfig gape_args_to_config(int argc, char **argv);

#endif
