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
struct gape_watch_cfg;

/**
 * Callback function defs
 */

// runs a watch check and returns true whenever the watch has triggered
typedef bool (*gape_watch_cond)(struct gape_watch *self,
                                struct gape_watch_cfg *watch_cfg);
// runs a watch action and retrns a status code (similar to program exit codes)
typedef int (*gape_watch_act)(struct gape_watch *self,
                              struct gape_watch_cfg *cfg);

typedef int (*gape_watch_out)(struct gape_watch *self,
                              struct gape_watch_cfg *cfg);

/**
 * Conditions
 */

struct gape_cond_time_sec {
  time_t seconds;
  time_t next_time;
};

struct gape_cond_time_sec gape_cond_time_sec_init(time_t seconds);

// a test condition that always returns true
bool gape_cond_true(struct gape_watch *self, struct gape_watch_cfg *watch_cfg);
// a test condition that always returns false
bool gape_cond_false(struct gape_watch *self, struct gape_watch_cfg *watch_cfg);
// regular timer-based watch
bool gape_cond_time_sec(struct gape_watch *self,
                        struct gape_watch_cfg *watch_cfg);

/**
 * Actions
 */

struct gape_act_exec {
  // program to run
  char *const path;
  // null terminated argv
  char *const *args;

  int status;
};

// an action that simply writes to a string buffer and exits after
// a single run
// watch_var should point to a char*
int gape_act_sprint(struct gape_watch *self, struct gape_watch_cfg *watch_cfg);

// call execve
int gape_act_exec(struct gape_watch *self, struct gape_watch_cfg *watch_cfg);

/**
 * Out functions
 */
int gape_out_none(struct gape_watch *self, struct gape_watch_cfg *watch_cfg);

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

/**
 * This struct simply holds shared references for all callbacks
 */
struct gape_watch_cfg {
  void *act_cfg;
  void *cond_cfg;
  void *out_cfg;

  // output buffers
  struct gape_buffer out_cur;
  struct gape_buffer out_prev;
};

struct gape_watch_cfg gape_watch_cfg_init(void *cond_cfg, void *act_cfg,
                                          void *out_cfg);
void gape_watch_cfg_swap_out(struct gape_watch_cfg *self);
void gape_watch_cfg_free(struct gape_watch_cfg *self);

struct gape_watch gape_watch_init(void);

// call to break out of watch loop early
void gape_watch_exit(struct gape_watch *self);

// execute a watch with a condition and action
int gape_watch(struct gape_watch *self, gape_watch_cond cond, void *cond_cfg,
               gape_watch_act act, void *act_cfg, gape_watch_out out,
               void *out_cfg);

void gape_watch_free(struct gape_watch *self);

#endif
