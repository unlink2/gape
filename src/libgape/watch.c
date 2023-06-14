#include "libgape/watch.h"
#include "libgape/buffer.h"
#include "libgape/error.h"
#include "libgape/log.h"
#include "libgape/macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

struct gape_cond_time_sec gape_cond_time_sec_init(time_t seconds) {
  struct gape_cond_time_sec self;
  memset(&self, 0, sizeof(self));
  self.seconds = seconds;

  self.next_time = time(NULL) + self.seconds;

  return self;
}

bool gape_cond_time_sec(struct gape_watch *self, void *_cfg) {
  struct gape_cond_time_sec *cfg = _cfg;
  gape_dbg_assert(cfg);

  time_t current = time(NULL);
  if (current > cfg->next_time) {
    cfg->next_time = current + cfg->seconds;
    return true;
  }

  return false;
}

bool gape_cond_true(struct gape_watch *self, void *cfg) {
  GAPE_UNUSED(self);
  GAPE_UNUSED(cfg);
  return true;
}

bool gape_cond_false(struct gape_watch *self, void *cfg) {
  GAPE_UNUSED(self);
  GAPE_UNUSED(cfg);
  return false;
}

int gape_act_sprint(struct gape_watch *self, void *cfg) {
  GAPE_UNUSED(self);
  sprintf(cfg, "ACT");
  return 0;
}

int gape_act_exec(struct gape_watch *self, void *_cfg) {
  struct gape_act_exec *cfg = _cfg;
  gape_dbg_assert(cfg);

  gape_buffer_free(&cfg->prev);
  cfg->prev = cfg->current;
  cfg->current = gape_buffer_init();

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

    // set ip pipes
    close(link[0]);
    close(link[1]);

    execv(cfg->path, cfg->args);
  } else {
    // parent proc
    close(link[1]);

    int64_t n_read = 0;
    do {
      // read from fd until end or error
      n_read = read(link[0], gape_buffer_next(&cfg->current, GAPE_BUFF_READ),
                    GAPE_BUFF_READ);
      if (n_read == -1) {
        gape_errno();
        return EXIT_FAILURE;
      }

      gape_buffer_adv(&cfg->current, n_read);
    } while (n_read);

    // obtain exit code for process
    waitpid(pid, &cfg->status, 0);
  }

  return cfg->status;
}

struct gape_watch gape_watch_init(void) {
  struct gape_watch self;
  memset(&self, 0, sizeof(self));
  self.n_runs = GAPE_NRUN_FOREVER;

  return self;
}

void gape_watch_exit(struct gape_watch *self) { self->n_runs = 0; }

int gape_watch(struct gape_watch *self, gape_watch_cond cond, void *cond_cfg,
               gape_watch_act act, void *act_cfg) {
  gape_dbg_assert(act);
  gape_dbg_assert(cond);

  int status = 0;

  while (self->n_runs == GAPE_NRUN_FOREVER || self->n_runs-- > 0) {
    while (!cond(self, cond_cfg)) {
      usleep(GAPE_SPIN_MS);
    }

    status = act(self, act_cfg);

    gape_dbg("Act exited with status %d\n", status);

    if (gape_err() || (self->exit_on_err && status != 0)) {
      break;
    }

    // TODO: Allow custom output callback
  }

  return status;
}

void gape_watch_free(struct gape_watch *self) {}
