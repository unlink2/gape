#include <libgape/macros.h>
#include <libgape/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char ERR_MESSAGE[GAPE_ERR_MSG_LEN];
_Thread_local char ERR_DETAILS[GAPE_ERR_DETAIL_LEN];
_Thread_local enum GapeError GAPE_ERR;

const char *gape_err_to_str(enum GapeError self) {
  switch (self) {
  case GAPE_OK:
    return "OK";
  }
}

void gape_err_set(enum GapeError err) { GAPE_ERR = err; }

enum GapeError gape_err(void) {
  enum GapeError latest = GAPE_ERR;
  return latest;
}

void gape_err_reset(void) { GAPE_ERR = GAPE_OK; }

char *gape_err_msg_ptr(void) { return ERR_MESSAGE; }

size_t gape_err_msg_len(void) { return GAPE_ERR_MSG_LEN; }

void *gape_err_details_ptr(void) { return ERR_DETAILS; }

size_t gape_err_details_len(void) { return GAPE_ERR_DETAIL_LEN; }
