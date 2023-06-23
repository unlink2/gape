#ifndef CONFIG_H_
#define CONFIG_H_

#include "libgape/watch.h"

enum gape_watch_stat {
  GAPE_WS_EXEC,
};

struct gape_config {
  const char *prg_path;
  char **prg_args;

  // internal use only. Pass ptr to sh -c argument that may have been malloced
  // here to have it freed in config_free
  char *_free_me_sh_c;

  gape_watch_cond cond;
  gape_watch_act act;
  gape_watch_out out;

  bool dry;

  enum gape_watch_stat strategy;
};

struct gape_config gape_config_init(void);

void gape_config_free(struct gape_config *self);

#endif
