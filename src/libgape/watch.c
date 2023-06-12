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
  sprintf(self->act_cfg, "ACT");
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

    if (!self->act(self)) {
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
