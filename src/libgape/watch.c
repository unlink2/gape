#include "libgape/watch.h"
#include "libgape/buffer.h"
#include "libgape/error.h"
#include "libgape/log.h"
#include "libgape/macros.h"
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "libgape/config.h"
#include <libgen.h>
#include <dirent.h>
#include <limits.h>
#include <ftw.h>
#include <fts.h>

struct gape_cond_cfg gape_cond_time_sec_init(time_t seconds) {
  struct gape_cond_cfg self;
  memset(&self, 0, sizeof(self));
  self.seconds = seconds;
  self.next_time = time(NULL);

  return self;
}

struct gape_cond_cfg gape_cond_fstat_init(const char *observe_path,
                                          int16_t max_depth, bool all) {
  struct gape_cond_cfg self = gape_cond_cfg_init();
  self.max_depth = max_depth;
  self.all = all;
  self.observe_path = observe_path;
  return self;
}

struct gape_cond_cfg gape_cond_cfg_init(void) {
  struct gape_cond_cfg self;
  memset(&self, 0, sizeof(self));
  return self;
}

bool gape_cond_time_sec(struct gape_watch *self) {
  struct gape_cond_cfg *cfg = &self->cond_cfg;
  gape_dbg_assert(cfg);

  time_t current = time(NULL);
  if (current > cfg->next_time) {
    cfg->next_time = current + cfg->seconds;
    return true;
  }

  return false;
}

// TODO: Implement realpath that does not use malloc
bool gape_should_fstat(struct gape_watch *self, char *path) {
  char *base = basename(path);
  bool basic_check = (self->cond_cfg.all || base[0] != '.') &&
                     strcmp(".", base) != 0 && strcmp("..", base) != 0;

  char real_path_src[PATH_MAX];
  realpath(path, real_path_src);

  char real_path_tmp[PATH_MAX];

  bool excluded = false;
  if (basic_check) {
    for (size_t i = 0; i < self->cond_cfg.ignore_paths->len; i++) {
      char *const p = gape_vec_get(self->cond_cfg.ignore_paths, i);
      if (!realpath(p, real_path_tmp)) {
        gape_errno();
        return false;
      }

      if (strncmp(real_path_src, real_path_tmp, PATH_MAX) == 0) {
        excluded = true;
        break;
      }
    }
  }

  if (excluded) {
    for (size_t i = 0; i < self->cond_cfg.include_paths->len; i++) {
      char *const p = gape_vec_get(self->cond_cfg.include_paths, i);
      if (!realpath(p, real_path_tmp)) {
        gape_errno();
        return false;
      }

      if (strncmp(real_path_src, real_path_tmp, PATH_MAX) == 0) {
        excluded = false;
        break;
      }
    }
  }

  return basic_check && !excluded;
}

// calculate fstat sum for condition checking
// TODO: This does call malloc(3) but it should not be a huge issue
// all in all
int64_t gape_fstat_sum(struct gape_watch *self, const char *path) {
  int sum = 0;

  if (!gape_should_fstat(self, (char *)path)) {
    return 0;
  }

  const char *const paths[] = {path, NULL};
  FTS *handle = fts_open((char *const *)paths, FTS_NOCHDIR, NULL);
  if (!handle) {
    gape_errno();
    return 0;
  }

  FTSENT *node = NULL;
  while ((node = fts_read(handle))) {
    int16_t depth = node->fts_level;

    if ((self->cond_cfg.max_depth != GAPE_STAT_DEPTH_INF &&
         depth > self->cond_cfg.max_depth) ||
        !gape_should_fstat(self, node->fts_path)) {
      fts_set(handle, node, FTS_SKIP);
      continue;
    }

    gape_dbg("Stating '%s' at depth %d\n", node->fts_path, depth);

    struct stat *sb = node->fts_statp;
    // calc stat
    // this is a bad implementation, but
    // it might just work for most cases
    sum += (int)sb->st_size;
    sum += (int)sb->st_atim.tv_nsec;
    sum += (int)sb->st_ctim.tv_nsec;
    sum += (int)sb->st_mtim.tv_nsec;
    sum += (int)sb->st_mode;
    sum += (int)sb->st_gid;
  }

  if (fts_close(handle)) {
    gape_errno();
  }

  return sum;
}

bool gape_cond_fstat_poll(struct gape_watch *self) {
  int64_t fstat_sum = gape_fstat_sum(self, self->cond_cfg.observe_path);
  int64_t fstat_sum_last = self->cond_cfg.fstat_sum_last;
  self->cond_cfg.fstat_sum_last = fstat_sum;

  gape_dbg("last stat: %ld. sum: %ld\n", fstat_sum_last, fstat_sum);

  return fstat_sum_last != fstat_sum;
}

bool gape_cond_true(struct gape_watch *self) {
  GAPE_UNUSED(self);
  return true;
}

bool gape_cond_false(struct gape_watch *self) {
  GAPE_UNUSED(self);
  return false;
}

struct gape_act_cfg gape_act_cfg_init(void) {
  struct gape_act_cfg self;
  memset(&self, 0, sizeof(self));
  return self;
}

struct gape_act_cfg gape_act_cfg_exec(const char *path, char *const *args) {
  struct gape_act_cfg self = gape_act_cfg_init();

  self.path = path;
  self.args = args;

  return self;
}

int gape_act_sprint(struct gape_watch *self) {
  GAPE_UNUSED(self);
  sprintf(self->act_cfg.custom, "ACT");
  return 0;
}

int gape_act_exec(struct gape_watch *self) {
  struct gape_act_cfg *cfg = &self->act_cfg;
  gape_dbg_assert(cfg);

  gape_buffer_clear(&self->out_cur);

  if (cfg->dry) {
    gape_dbg("Path: %s\nargs:\n\n", cfg->path);
    char *const *arg = cfg->args;
    while (*arg) {
      gape_dbg("%s\n", *arg);
      arg++;
    }
    gape_watch_exit(self);
    return 0;
  }

  gape_watch_swap_out(self);

  int link[2];
  if (pipe(link) == -1) {
    gape_err_set(GAPE_ERR_PIPE);
    return true;
  }

  pid_t pid = fork();
  if (pid == -1) {
    gape_err_set(GAPE_ERR_FORK);
    return true;
  }

  if (pid == 0) {
    // child proc
    dup2(link[1], STDOUT_FILENO);

    // set up pipes
    close(link[0]);
    close(link[1]);

    execv(cfg->path, cfg->args);
    // this should never be called!
    gape_panic("'%s': No such file or directory\n", cfg->path);
  } else {
    // parent proc
    close(link[1]);
    // wait for child to exit and obtain exit code
    waitpid(pid, &cfg->status, 0);

    int64_t n_read = 0;
    do {
      // read from fd until end or error
      uint8_t *next = gape_buffer_next(&self->out_cur, GAPE_BUFF_READ);
      gape_dbg_assert(next);

      n_read = read(link[0], next, GAPE_BUFF_READ);
      if (n_read == -1) {
        gape_errno();
        return EXIT_FAILURE;
      }

      gape_buffer_adv(&self->out_cur, n_read);
    } while (n_read);
  }

  return cfg->status;
}

struct gape_out_cfg gape_out_cfg_init(void) {
  struct gape_out_cfg self;
  memset(&self, 0, sizeof(self));
  return self;
}

int gape_out_none(struct gape_watch *self) {
  GAPE_UNUSED(self);

  return 0;
}

int gape_out_print(struct gape_watch *self) {
  size_t len = gape_buffer_len(&self->out_cur);
  if (len) {
    // TODO: maybe allow writing to any file here
    return (int)write(fileno(stdout), gape_buffer_start(&self->out_cur), len);
  }

  return 0;
}

struct gape_watch
gape_watch_init(gape_watch_cond cond, struct gape_cond_cfg cond_cfg,
                gape_watch_act act, struct gape_act_cfg act_cfg,
                gape_watch_out out, struct gape_out_cfg out_cfg

) {
  struct gape_watch self;
  memset(&self, 0, sizeof(self));
  self.n_runs = GAPE_NRUN_FOREVER;

  self.out = out;
  self.out_cfg = out_cfg;
  self.act = act;
  self.act_cfg = act_cfg;
  self.cond = cond;
  self.cond_cfg = cond_cfg;

  self.out_cur = gape_buffer_init();
  self.out_prev = gape_buffer_init();

  return self;
}

// TODO: differentiate between differnet types of condition
struct gape_watch gape_watch_from_cfg(struct gape_config *cfg) {

  gape_watch_cond cond = gape_cond_false;
  struct gape_cond_cfg cond_cfg = gape_cond_cfg_init();

  if (cfg->observe_path) {
    cond = gape_cond_fstat_poll;

    int16_t max_depth = cfg->max_depth;

    if (cfg->recursive) {
      max_depth = GAPE_STAT_DEPTH_INF;
    } else if (max_depth == 0) {
      max_depth = 1;
    }

    cond_cfg = gape_cond_fstat_init(cfg->observe_path, max_depth, cfg->all);

    cond_cfg.ignore_paths = &cfg->ignore_paths;
    cond_cfg.include_paths = &cfg->include_paths;

    gape_dbg("Observing %s with a depth of %d (dotfiles: %d)\n",
             cond_cfg.observe_path, cond_cfg.max_depth, cond_cfg.all);
  } else {
    gape_dbg("Running program every %d seconds.", cfg->interval);
    cond = gape_cond_time_sec;
    cond_cfg = gape_cond_time_sec_init(cfg->interval);
  }

  struct gape_watch self =
      gape_watch_init(cond, cond_cfg, gape_act_exec,
                      gape_act_cfg_exec(cfg->prg_path, cfg->prg_args),
                      gape_out_print, gape_out_cfg_init());

  self.act_cfg.dry = cfg->dry;
  self.usleep = cfg->usleep;
  self.n_runs = cfg->n_runs;

  gape_dbg("usleep: %ld. n_runs: %d\n", self.usleep, self.n_runs);

  return self;
}

void gape_watch_swap_out(struct gape_watch *self) {
  struct gape_buffer tmp = self->out_prev;
  self->out_prev = self->out_cur;
  self->out_cur = tmp;
}

void gape_watch_exit(struct gape_watch *self) { self->n_runs = 0; }

int gape_watch(struct gape_watch *self) {

  gape_dbg_assert(self->act);
  gape_dbg_assert(self->cond);
  gape_dbg_assert(self->out);

  int status = 0;

  while (self->n_runs == GAPE_NRUN_FOREVER || self->n_runs-- > 0) {
    while (!self->cond(self)) {
      usleep(self->usleep);
    }

    status = self->act(self);

    gape_dbg("Act exited with status %d\n", status);

    if (gape_err() || (self->exit_on_err && status != 0)) {
      break;
    }

    self->out(self);
  }

  return status;
}

void gape_watch_free(struct gape_watch *self) {
  gape_buffer_free(&self->out_cur);
  gape_buffer_free(&self->out_prev);
}
