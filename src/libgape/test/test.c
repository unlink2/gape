#include "libgape/test/test.h"
#include "libgape/test/watch.h"
#include <cmocka.h>

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_watch_true),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
