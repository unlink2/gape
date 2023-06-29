#include "libgape/test/test.h"
#include "libgape/test/vec.h"
#include "libgape/vec.h"

void test_vec(void **state) {
  struct gape_vec vec = gape_vec_init(sizeof(int));

  for (int i = 0; i < GAPE_VEC_INITIAL_LEN * 4; i++) {
    gape_vec_add(&vec, &i);
  }

  for (int i = 0; i < GAPE_VEC_INITIAL_LEN * 4; i++) {
    assert_int_equal(i, *(int *)gape_vec_get(&vec, i));
  }

  gape_vec_free(&vec);
}
