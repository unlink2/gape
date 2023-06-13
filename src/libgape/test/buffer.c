#include "libgape/test/buffer.h"
#include "libgape/error.h"
#include "libgape/test/test.h"
#include "libgape/buffer.h"
#include <cmocka.h>

void test_buffer(void **state) {
  struct GapeBuffer b1 = gape_buffer_init();

  assert_non_null(gape_buffer_next(&b1, GAPE_BUFFER_INITIAL_LEN - 1));
  assert_false(gape_err());
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN, b1.max_len);
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN - 1, b1.index);

  assert_non_null(gape_buffer_next(&b1, 127));
  assert_false(gape_err());
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN + 127 * 2, b1.max_len);
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN - 1 + 127, b1.index);

  assert_non_null(gape_buffer_next(&b1, 127));
  assert_false(gape_err());
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN + 127 * 2, b1.max_len);
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN - 1 + 127 * 2, b1.index);

  assert_non_null(gape_buffer_next(&b1, 127));
  assert_false(gape_err());
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN + 127 * 4, b1.max_len);
  assert_int_equal(GAPE_BUFFER_INITIAL_LEN - 1 + 127 * 3, b1.index);

  gape_buffer_free(&b1);
}
