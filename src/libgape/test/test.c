#include "libgape/test/test.h"
#include "libgape/test/watch.h"
#include "libgape/test/buffer.h"
#include "libgape/test/vec.h"
#include <cmocka.h>

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_watch_true),
      cmocka_unit_test(test_buffer),
      cmocka_unit_test(test_vec),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
