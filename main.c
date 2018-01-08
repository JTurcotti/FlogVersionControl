#include "porcelain.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Insufficient arguments\n");
    exit(1);
  } else if (!strcmp(argv[1], "init")) {
    flog_init();
  } else if (!strcmp(argv[1], "log")) {
    flog_log();
  } else if (!strcmp(argv[1], "commit")) {
    if (argc >= 3)
      flog_commit(argv[2]);
    else
      flog_commit(NULL);
  } else if (!strcmp(argv[1], "checkout")) {
    if (argc >= 3)
      flog_checkout(argv[2]);
    else
      fprintf(stderr, "Insufficient arguments\n");
  } else if (!strcmp(argv[1], "add")) {
    if (argc < 3) {
      fprintf(stderr, "Insufficient arguments\n");
    } else {
      flog_add(argv[2]);
    }
  } else {
    fprintf(stderr, "Unrecognized command: %s\n", argv[1]);
  }
  return 0;   
}



