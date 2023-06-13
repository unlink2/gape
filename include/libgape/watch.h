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

struct GapeWatch;

/**
 * Callback function defs
 */

// runs a watch check and returns true whenever the watch has triggered
typedef bool (*GapeWatchCond)(struct GapeWatch *self);
// runs a watch action and retrns false if the watch should exit
typedef bool (*GapeWatchAct)(struct GapeWatch *self);

/**
 * Conditions
 */

struct GapeCondTimeSec {
  time_t seconds;
  time_t next_time;
};

struct GapeCondTimeSec gape_cond_time_sec_init(time_t seconds);

// a test condition that always returns true
bool gape_cond_true(struct GapeWatch *self);
// a test condition that always returns false
bool gape_cond_false(struct GapeWatch *self);
// regular timer-based watch
bool gape_cond_time_sec(struct GapeWatch *self);

/**
 * Actions
 */

struct GapeActExec {
  // program to run
  char *const path;
  // null terminated argv
  char *const *args;

  int status;
  struct GapeBuffer current;
  struct GapeBuffer prev;
};

// an action that simply writes to a string buffer and exits after
// a single run
// watch_var should point to a char*
bool gape_act_sprint(struct GapeWatch *self);

// call execve
bool gape_act_exec(struct GapeWatch *self);

// Contains shared config for all action types
struct GapeWatch {
  GapeWatchCond cond;
  GapeWatchAct act;

  // number of runs before force exiting
  // if set to -1 run forever
  int32_t n_runs;

  // watch cond config
  void *cond_cfg;

  // watch act config
  void *act_cfg;
};

struct GapeWatch gape_watch_init(void);

void gape_watch_set_cond(struct GapeWatch *self, GapeWatchCond cond, void *cfg);
void gape_watch_set_act(struct GapeWatch *self, GapeWatchAct act, void *cfg);

void gape_watch(struct GapeWatch *self);

void gape_watch_free(struct GapeWatch *self);

#endif
