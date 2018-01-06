#include "plumbing.h"

user_t data = {"Rasputin", "GrigoriR@kremlin.gov"};
user_t *global_author = &data;

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

int flog_init() {
  if (dir_exists(MAIN_LOC)) {
    fprintf(stderr, "Flog repo already exists here\n");
  } else if (mkdir(MAIN_LOC, DEFFILEMODE) ||
	     mkdir(OBJECT_LOC, DEFFILEMODE) ||
	     mkdir(REF_LOC, DEFFILEMODE) ||
	     mkdir(BRANCH_LOC, DEFFILEMODE)) {
    perror("Error creating flog repo");
    return -1;
  } else {
    printf("Initialized empty flog repo\n");
    return 0;
  }
}

int flog_commit(char *msg) {
  hash_t tree = index_build();
  if (!tree) {
    exit(1);
  }
  printf("Built index to tree: '%s'\n", tree);

  if (access(HEAD_LOC, R_OK)) {
    //HEAD symbolic ref dne
    hash_t sha = make_commit(tree, ROOT_COMMIT, global_author, msg);
    write_whole_file(BRANCH_PATH(master), sha);
    write_whole_file(HEAD_LOC, BRANCH_PATH(master));
    printf("Successful initial commit: '%s'\n", sha);
    return 0;
  } else {
    hash_t parent_sha = read_whole_file(read_whole_file(HEAD_LOC));
    hash_t sha = make_commit(tree, parent_sha, global_author, msg);
    write_whole_file(BRANCH_PATH(master), sha);
    printf("Successful commit: '%s'\n", sha);
  }
  
}
  
