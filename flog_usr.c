#include "plumbing.h"

int flog_add(char *filename) {
  hash_t sha;
  if (!(sha = make_blob(filename))) {
    fprintf(stderr, "Add operation failed\n");
    return -1;
  } else if (!strcmp(sha, EBLOB_EXIST)) {
    fprintf(stderr, "File '%s' matches preexisting blob; no changes to add\n", filename);
  } else {
    index_addblob(sha, filename);
  }
}

int flog_commit(char *msg) {
  hash_t sha = index_build();
  if (sha) {
    printf("Built index to tree: '%s'\n", sha);
  }
}
  
