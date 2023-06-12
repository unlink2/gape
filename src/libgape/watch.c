#include "libgape/watch.h"
#include "libgape/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool gape_watch_cond_true(struct GapeWatch *self) {
  GAPE_UNUSED(self);
  return true;
}

bool gape_watch_cond_false(struct GapeWatch *self) {
  GAPE_UNUSED(self);
  return false;
}

bool gape_watch_act_sprint(struct GapeWatch *self) {
  sprintf(self->watch_var, "ACT");
  return true;
}

struct GapeWatch gape_watch_init(void) {
  struct GapeWatch self;
  memset(&self, 0, sizeof(self));
  self.n_runs = GAPE_NRUN_FOREVER;

  return self;
}
struct GapeWatch gape_watch_true_init(GapeWatchAct act, void *watch_cfg,
                                      void *watch_var) {
  struct GapeWatch self = gape_watch_init();
  self.cond = gape_watch_cond_true;
  self.action = act;
  self.watch_var = watch_var;
  self.watch_cfg = watch_cfg;
  return self;
}

struct GapeWatch gape_watch_true_false(GapeWatchAct act, void *watch_cfg,
                                       void *watch_var) {
  struct GapeWatch self = gape_watch_init();
  self.cond = gape_watch_cond_false;
  self.action = act;
  self.watch_var = watch_var;
  self.watch_cfg = watch_cfg;
  return self;
}

struct GapeWatch gape_watch_time_init(uint64_t seconds) {
  struct GapeWatch self = gape_watch_init();

  return self;
}

void gape_watch(struct GapeWatch *self) {
  gape_dbg_assert(self->action);
  gape_dbg_assert(self->cond);
  while (self->n_runs == GAPE_NRUN_FOREVER || self->n_runs-- > 0) {
    while (!self->cond(self)) {
      usleep(GAPE_SPIN_MS);
    }

    if (!self->action(self)) {
      break;
    }
  }
}

void gape_watch_free(struct GapeWatch *self) {
  if (self->watch_cfg) {
    free(self->watch_cfg);
  }

  if (self->watch_var) {
    free(self->watch_var);
  }
}
