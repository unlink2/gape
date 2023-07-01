#ifndef CONFIG_H_
#define CONFIG_H_

#include "libgape/watch.h"
#include "libgape/vec.h"

struct gape_config {
  struct gape_vec ignore_paths;
  struct gape_vec include_paths;

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

  bool diff;
  const char *diff_prg;
  const char *tmp_cur_path;
  const char *tmp_prev_path;

  bool dry;
};

struct gape_config gape_config_init(void);

void gape_config_free(struct gape_config *self);

#endif
