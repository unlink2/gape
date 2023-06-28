#ifndef CONFIG_H_
#define CONFIG_H_

#include "libgape/watch.h"

struct gape_config {
  const char *prg_path;
  char **prg_args;

  // internal use only. Pass ptr to sh -c argument that may have been malloced
  // here to have it freed in config_free
  char *_free_me_sh_c;

  int interval;

  const char *observe_path;
  int16_t max_depth;
  bool recursive;
  bool all;

  int64_t usleep;
  int32_t n_runs;

  bool dry;
};

struct gape_config gape_config_init(void);

void gape_config_free(struct gape_config *self);

#endif
