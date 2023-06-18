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

bool gape_cond_time_sec(struct gape_watch *self) {
  struct gape_cond_time_sec *cfg = self->cond_cfg;
  gape_dbg_assert(cfg);

  time_t current = time(NULL);
  if (current > cfg->next_time) {
    cfg->next_time = current + cfg->seconds;
    return true;
  }

  return false;
}

bool gape_cond_true(struct gape_watch *self) {
  GAPE_UNUSED(self);
  return true;
}

bool gape_cond_false(struct gape_watch *self) {
  GAPE_UNUSED(self);
  return false;
}

int gape_act_sprint(struct gape_watch *self) {
  GAPE_UNUSED(self);
  sprintf(self->act_cfg, "ACT");
  return 0;
}

int gape_act_exec(struct gape_watch *self) {
  struct gape_act_exec *cfg = self->act_cfg;
  gape_dbg_assert(cfg);

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
      n_read = read(link[0], gape_buffer_next(&self->out_cur, GAPE_BUFF_READ),
                    GAPE_BUFF_READ);
      if (n_read == -1) {
        gape_errno();
        return EXIT_FAILURE;
      }

      gape_buffer_adv(&self->out_cur, n_read);
    } while (n_read);

    // obtain exit code for process
    waitpid(pid, &cfg->status, 0);
  }

  return cfg->status;
}

int gape_out_none(struct gape_watch *self) {
  GAPE_UNUSED(self);

  return 0;
}

int gape_out_print(struct gape_watch *self) {
  gape_buffer_null_term(&self->out_cur);

  // TODO: maybe allow writing to any file here
  return (int)write(fileno(stdout), gape_buffer_start(&self->out_cur),
                    gape_buffer_len(&self->out_cur));
}

struct gape_watch gape_watch_init(gape_watch_cond cond, void *cond_cfg,
                                  gape_watch_act act, void *act_cfg,
                                  gape_watch_out out, void *out_cfg

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
      usleep(GAPE_SPIN_MS);
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
