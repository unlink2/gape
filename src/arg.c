#include "arg.h"
#include "libgape/buffer.h"
#include "libgape/config.h"
#include "libgape/log.h"
#include <argtable2.h>
#include <stdint.h>
#include <string.h>

#define GAPE_SHELL_CMD "/bin/sh"
#define GAPE_SHELL_OPT "-c"
#define GAPE_ENV_CMD "/usr/bin/env"

struct gape_config gape_args_to_config(int argc, char **argv) {
  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;

  struct arg_lit *prg_exec_no_sh = NULL;
  struct arg_str *prg_exec = NULL;

  struct arg_lit *dry = NULL;

  struct arg_int *interval = NULL;

  struct arg_file *observe_path = NULL;
  struct arg_int *max_depth = NULL;
  struct arg_lit *all = NULL;
  struct arg_lit *recursive = NULL;

  // arg end stores errors
  struct arg_end *end = NULL;

  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
      verb = arg_litn("v", "verbose", 0, GAPE_LOG_LEVEL_DBG, "verbose output"),
      prg_exec = arg_strn(NULL, NULL, "COMMAND", 1, argc, "Command to execute"),
      prg_exec_no_sh = arg_lit0(
          "e", "exec",
          "Run command directly using exec instead of using '" GAPE_SHELL_CMD
          " " GAPE_SHELL_OPT "'"),
      dry = arg_lit0(NULL, "dry", "Do a test run"),
      interval = arg_int0("n", "interval", "seconds",
                          "Specify the update interval in seconds."),

      observe_path = arg_file0("o", "observe", "PATH",
                               "Observe a file or directory for changes"),
      max_depth = arg_int0("d", "depth", "DEPTH", "Max recursion depth"),
      recursive = arg_lit0("r", "recursive", "Observe path recursively"),
      all = arg_lit0("a", "all", "Observe dotfiles"),

      end = arg_end(20),
  };

  // output params
  char progname[] = "gape";
  char short_desc[] = "Gape - a watch-like program with extra features";

  // version info
  int version_major = 0;
  int version_minor = 0;
  int version_patch = 1;

  int nerrors = arg_parse(argc, argv, argtable);
  int exitcode = 0;

  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf("%s\n\n", short_desc);
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    goto exit;
  }

  if (version->count) {
    printf("%s version %d.%d.%d\n", progname, version_major, version_minor,
           version_patch);
    goto exit;
  }

  if (verb->count > 0) {
    gape_log_init(verb->count);
  } else {
    gape_log_init(GAPE_LOG_LEVEL_ERROR);
  }

  if (nerrors > 0) {
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  struct gape_config cfg = gape_config_init();

  cfg.dry = dry->count > 0;

  if (interval->count > 0) {
    cfg.interval = *interval->ival;
  } else {
    cfg.interval = 2;
  }

  cfg.recursive = recursive->count > 0;
  cfg.all = all->count > 0;

  if (max_depth->count > 0) {
    cfg.max_depth = (int16_t)max_depth->ival[max_depth->count - 1];
  }

  if (observe_path->count > 0) {
    cfg.observe_path = observe_path->filename[observe_path->count - 1];
  }

  // start set exec command
  // if exec is not set simply start prg_args with 2 'sh -c'
  if (prg_exec_no_sh->count == 0) {

    // 3 args + 1 for NULL
    cfg.prg_args = malloc(sizeof(const char *) * (3 + 1));

    cfg.prg_args[0] = GAPE_SHELL_CMD;
    cfg.prg_args[1] = GAPE_SHELL_OPT;

    struct gape_buffer sh_cmd = gape_buffer_init();
    for (size_t i = 0; i < prg_exec->count; i++) {
      size_t len = strlen(prg_exec->sval[i]);
      uint8_t *next = gape_buffer_next(&sh_cmd, len);

      memcpy(next, prg_exec->sval[i], len);

      gape_buffer_adv(&sh_cmd, len);

      // space between all the commands
      *gape_buffer_next(&sh_cmd, 1) = ' ';
      gape_buffer_adv(&sh_cmd, 1);
    }
    gape_buffer_null_term(&sh_cmd);
    cfg.prg_args[2] = (char *)gape_buffer_move(&sh_cmd);

    // make sure this string can be freed later
    cfg._free_me_sh_c = cfg.prg_args[2];

    cfg.prg_args[3] = NULL;
  } else {
    // 1 for env + args + 1 for NULL
    cfg.prg_args = malloc(sizeof(const char *) * (1 + prg_exec->count + 1));

    cfg.prg_args[0] = GAPE_ENV_CMD;
    for (size_t i = 1; i < prg_exec->count + 1; i++) {
      // FIXME: this cast is pretty bad
      cfg.prg_args[i] = (char *)prg_exec->sval[i - 1];
    }
    cfg.prg_args[prg_exec->count + 1] = NULL;
  }
  // the prg path is the 0th arg
  cfg.prg_path = cfg.prg_args[0];

  // end set exec command

  // map args to cfg data here
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return cfg;
exit:
  // TODO: do not exit here, just set an error
  exit(exitcode); // NOLINT
}
