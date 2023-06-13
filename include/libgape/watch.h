#ifndef WATCH_H_
#define WATCH_H_

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include "libgape/buffer.h"
#include "libgape/macros.h"

#define GAPE_NRUN_FOREVER -1
#define GAPE_SPIN_MS 100
#define GAPE_BUFF_READ 256

struct gape_watch;

/**
 * Callback function defs
 */

// runs a watch check and returns true whenever the watch has triggered
typedef bool (*gape_watch_cond)(struct gape_watch *self, void *cfg);
// runs a watch action and retrns a status code (similar to program exit codes)
typedef int (*gape_watch_act)(struct gape_watch *self, void *cfg);

/**
 * Conditions
 */

struct GapeCondTimeSec {
  time_t seconds;
  time_t next_time;
};

struct GapeCondTimeSec gape_cond_time_sec_init(time_t seconds);

// a test condition that always returns true
bool gape_cond_true(struct gape_watch *self, void *cfg);
// a test condition that always returns false
bool gape_cond_false(struct gape_watch *self, void *cfg);
// regular timer-based watch
bool gape_cond_time_sec(struct gape_watch *self, void *cfg);

/**
 * Actions
 */

struct GapeActExec {
  // program to run
  char *const path;
  // null terminated argv
  char *const *args;

  int status;
  struct gape_buffer current;
  struct gape_buffer prev;
};

// an action that simply writes to a string buffer and exits after
// a single run
// watch_var should point to a char*
int gape_act_sprint(struct gape_watch *self, void *cfg);

// call execve
int gape_act_exec(struct gape_watch *self, void *cfg);

/**
 * Watch config
 */

// Contains shared config for all action types
struct gape_watch {
  // number of runs before force exiting
  // if set to -1 run forever
  int32_t n_runs;

  // exit on status != 0
  bool exit_on_err;
};

struct gape_watch gape_watch_init(void);

// call to break out of watch loop early
void gape_watch_exit(struct gape_watch *self);

// execute a watch with a condition and action
int gape_watch(struct gape_watch *self, gape_watch_cond cond, void *cond_cfg,
               gape_watch_act act, void *act_cfg);

void gape_watch_free(struct gape_watch *self);

#endif
