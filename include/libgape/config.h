#ifndef CONFIG_H_
#define CONFIG_H_

#include "libgape/watch.h"

enum gape_watch_stat {
  GAPE_WS_EXEC,
};

struct gape_config {
  const char *prg_path;
  char **prg_args;

  const char *exec;
  struct gape_buffer exec_args;

  gape_watch_cond cond;
  gape_watch_act act;
  gape_watch_out out;

  bool dry;

  enum gape_watch_stat strategy;
};

struct gape_config gape_config_init(void);

void gape_config_exec(struct gape_config *self, const char *prg,
                      const char **args, size_t args_len);

void gape_config_free(struct gape_config *self);

#endif
