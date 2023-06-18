#include "arg.h"
#include "libgape/config.h"
#include "libgape/log.h"

#define GAPE_SHELL_CMD "sh"
#define GAPE_SHELL_OPT "-c"

struct gape_config gape_args_to_config(int argc, char **argv) {
  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;

  struct arg_lit *prg_exec_no_sh = NULL;
  struct arg_str *prg_exec = NULL;

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

  // start set exec command
  int prg_offset = 0;
  // if exec is not set simply start prg_args with 2 'sh -c'
  if (prg_exec_no_sh->count == 0) {
    prg_offset = 2;
  }

  cfg.prg_args =
      malloc(sizeof(const char *) * (prg_exec->count + 1 + prg_offset));

  if (prg_offset > 0) {
    cfg.prg_args[0] = GAPE_SHELL_CMD;
    cfg.prg_args[1] = GAPE_SHELL_OPT;
  }

  for (size_t i = prg_offset; i < prg_exec->count + prg_offset; i++) {
    cfg.prg_args[i] = prg_exec->sval[i - prg_offset];
  }
  cfg.prg_args[prg_exec->count + prg_offset] = NULL;

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
