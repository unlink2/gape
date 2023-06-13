#include "libgape/watch.h"
#include "libgape/buffer.h"
#include "libgape/error.h"
#include "libgape/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

struct GapeCondTimeSec gape_cond_time_sec_init(time_t seconds) {
  struct GapeCondTimeSec self;
  memset(&self, 0, sizeof(self));
  self.seconds = seconds;

  self.next_time = time(NULL) + self.seconds;

  return self;
}

bool gape_cond_time_sec(struct GapeWatch *self) {
  struct GapeCondTimeSec *cfg = self->cond_cfg;
  gape_dbg_assert(cfg);

  time_t current = time(NULL);
  if (current > cfg->next_time) {
    cfg->next_time = current + cfg->seconds;
    return true;
  }

  return false;
}

bool gape_cond_true(struct GapeWatch *self) {
  GAPE_UNUSED(self);
  return true;
}

bool gape_cond_false(struct GapeWatch *self) {
  GAPE_UNUSED(self);
  return false;
}

bool gape_act_sprint(struct GapeWatch *self) {
  sprintf(self->act_cfg, "ACT");
  return true;
}

bool gape_act_exec(struct GapeWatch *self) {

  struct GapeActExec *cfg = self->act_cfg;
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
        return true;
      }

      gape_buffer_adv(&cfg->current, n_read);
    } while (n_read);

    // obtain exit code for process
    waitpid(pid, &cfg->status, 0);
  }

  return true;
}

struct GapeWatch gape_watch_init(void) {
  struct GapeWatch self;
  memset(&self, 0, sizeof(self));
  self.n_runs = GAPE_NRUN_FOREVER;

  return self;
}

void gape_watch(struct GapeWatch *self) {
  gape_dbg_assert(self->act);
  gape_dbg_assert(self->cond);
  while (self->n_runs == GAPE_NRUN_FOREVER || self->n_runs-- > 0) {
    while (!self->cond(self)) {
      usleep(GAPE_SPIN_MS);
    }

    if (!self->act(self) || gape_err()) {
      break;
    }
  }
}

void gape_watch_set_cond(struct GapeWatch *self, GapeWatchCond cond,
                         void *cfg) {
  self->cond = cond;
  self->cond_cfg = cfg;
}

void gape_watch_set_act(struct GapeWatch *self, GapeWatchAct act, void *cfg) {
  self->act = act;
  self->act_cfg = cfg;
}

void gape_watch_free(struct GapeWatch *self) {}
