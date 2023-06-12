#ifndef ERROR_H_
#define ERROR_H_

#include "libgape/macros.h"
#include <stddef.h>

/**
 * Error is a error type struct that provides
 * an errno-style interface.
 * It provides a generic message location as well as a details buffer
 * where additional error information can  be stored.
 * Both of those locations are thread local and therefore
 * are OK to overwirte at any point.
 * It is up to the caller to handle and read the buffers if
 * an error is returned. Do not rely on the buffer not being overwirtten after
 * making a new call! The error info struct should be included in structs.
 */

#define GAPE_ERR_MSG_LEN 1024
#define GAPE_ERR_DETAIL_LEN 1024

// Possible error types.
// Some may or may not require special error info
enum GapeError {
  GAPE_OK = 0,
};

const char *gape_err_to_str(enum GapeError self);

#define gape_err_msg(...) sprintf(gape_err_msg_ptr(), __VA_ARGS__);

#define gape_err_detail(data, len) memcpy(gape_err_details_ptr(), data, len);

// sets err to a value
// to indicate a failure in a call
void gape_err_set(enum GapeError err);

// return error code and resets value
// to OK
enum GapeError gape_err(void);
void gape_err_rest(void);

// the error message is in practice a thread local
// string buffer where errors may write specific information
// about the kind of failure. It should only ever be obtained
// via this function.
// This of course means that subsequent errors may overwrite
// the previous error message! Ensure to handle errors in a timely manner
// to avoid losing error information
char *gape_err_msg_ptr(void);
size_t gape_err_msg_len(void);

// Error details can be used to store metadata about the error
// The data type depends on the error kind and should be documented above
// if available
void *gape_err_details_ptr(void);
size_t gape_err_details_len(void);

// bail on error macro
#define gape_ok_or(err, ret)                                                   \
  {                                                                            \
    if ((err)) {                                                               \
      return (ret);                                                            \
    }                                                                          \
  }

// optional struct
#define gape_some_or(optional, ret)                                            \
  {                                                                            \
    if ((optional)) {                                                          \
      return (ret);                                                            \
    }                                                                          \
  }

#endif
