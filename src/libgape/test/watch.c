#include "libgape/test/watch.h"
#include "libgape/error.h"
#include "libgape/watch.h"
#include "libgape/test/test.h"
#include <cmocka.h>

void test_watch_true(void **state) {
  char dst[128];
  struct gape_act_cfg act_cfg = gape_act_cfg_init();
  act_cfg.custom = dst;
  struct gape_watch w1 =
      gape_watch_init(gape_cond_true, gape_cond_cfg_init(), gape_act_sprint,
                      act_cfg, gape_out_none, gape_out_cfg_init());

  w1.n_runs = 1;
  gape_watch(&w1);

  gape_watch_free(&w1);

  assert_string_equal("ACT", dst);
  assert_false(gape_err());
}
