#include "porcelain.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Insufficient arguments\n");
    exit(1);
  } else if (!strcmp(argv[1], "init")) {
    flog_init();
  } else if (!strcmp(argv[1], "log")) {
    flog_log();
  } else if (!strcmp(argv[1], "branch")) {
    if (argc >= 4 && !strcmp(argv[2], "-d"))
      flog_branch_delete(argv[3]);
    else if (argc >= 3)
      flog_branch(argv[2]);
    else
      flog_branch_list();
  } else if (!strcmp(argv[1], "commit")) {
    if (argc >= 4 && !strcmp(argv[2], "-m"))
      flog_commit(argv[3]);
    else
      flog_commit(NULL);
  } else if (!strcmp(argv[1], "checkout")) {
    if (argc >= 4 && !strcmp(argv[2], "-b")) {
      flog_branch(argv[3]);
      flog_checkout(argv[3]);
    } else if (argc >= 3)
      flog_checkout(argv[2]);
    else
      fprintf(stderr, "Insufficient arguments\n");
  } else if (!strcmp(argv[1], "add")) {
    if (argc < 3)
      fprintf(stderr, "Insufficient arguments\n");
    else
      flog_add(argv[2]);
  } else if (!strcmp(argv[1], "reset")) {
    if (argc < 3)
      flog_reset("HEAD", 1);
    else if (!strcmp(argv[2], "--hard"))
      if (argc >= 4)
	flog_reset(argv[3], 2);
      else
	flog_reset("HEAD", 2);
    else if (!strcmp(argv[2], "--soft"))
      if (argc >= 4)
	flog_reset(argv[3], 0);
      else
	flog_reset("HEAD", 0);
    else
      flog_reset(argv[2], 1);
  } else {
    fprintf(stderr, "Unrecognized command: %s\n", argv[1]);
  }
  return 0;   
}



