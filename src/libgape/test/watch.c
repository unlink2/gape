#include "libgape/test/watch.h"
#include "libgape/error.h"
#include "libgape/watch.h"
#include "libgape/test/test.h"
#include <cmocka.h>

void test_watch_true(void **state) {
  char dst[128];
  struct GapeWatch w1 = gape_watch_init();

  w1.n_runs = 1;
  gape_watch(&w1, gape_cond_true, NULL, gape_act_sprint, dst);

  assert_string_equal("ACT", dst);
  assert_false(gape_err());
}
