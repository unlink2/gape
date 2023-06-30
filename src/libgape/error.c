#include <libgape/macros.h>
#include <libgape/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char ERR_MESSAGE[GAPE_ERR_MSG_LEN];
_Thread_local char ERR_DETAILS[GAPE_ERR_DETAIL_LEN];
_Thread_local enum gape_error GAPE_ERR;

const char *gape_err_to_str(enum gape_error self) {
  switch (self) {
  case GAPE_OK:
    return "OK";
  case GAPE_ERRNO:
    return strerror(errno); // NOLINT
  default:
    return "Unknown";
  }
}

void gape_err_set(enum gape_error err) { GAPE_ERR = err; }

enum gape_error gape_err(void) {
  enum gape_error latest = GAPE_ERR;
  return latest;
}

void gape_err_reset(void) { GAPE_ERR = GAPE_OK; }

char *gape_err_msg_ptr(void) { return ERR_MESSAGE; }

size_t gape_err_msg_len(void) { return GAPE_ERR_MSG_LEN; }

void *gape_err_details_ptr(void) { return ERR_DETAILS; }

size_t gape_err_details_len(void) { return GAPE_ERR_DETAIL_LEN; }
