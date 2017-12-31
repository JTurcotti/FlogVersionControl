#include "plumbing.h"
  
//hashes a file to create a blob tying the filename to hash (used for storage file)
char *make_blob(char *filename) {
  char *contents = read_whole_file(filename);
  
  int n = strlen(contents);
  char str[n + 32];
  sprintf(str, "blob %d %s", n, contents);

  //hashes the generated str and converts string from base 256 to base 16
  char *hashed_name = shatos(SHA1(str, strlen(str), NULL));

  char dir[3];
  strncpy(dir, hashed_name, 2);
  dir[2] = '\0';
  char file_path[64], dir_path[64];
  //first two characters form dir name, remaining form filename
  sprintf(file_path, "%s/%s/%s", OBJECT_LOC, dir, hashed_name + 2);
  sprintf(dir_path, "%s/%s", OBJECT_LOC, dir);

  
  if (!dir_exists(OBJECT_LOC)) {
    fprintf(stderr, "Must initialize flog repo before attempting to write\n");
    exit(1);
  } else if (mkdir(dir_path, DEFFILEMODE) && errno != EEXIST) {//ok if already exists
    fprintf(stderr, "Error creating object-specific dir '%s': %s\n", dir_path, strerror(errno));
    return NULL;
  } else {
    write_whole_file(file_path, contents);
    printf("Succesfully wrote blob '%s'\n", hashed_name);
    return hashed_name;
  }
}

//indexes a blob called sha representing a file at path and returns 0, or fails and returns -1
int index_blob(char *sha, char *path) {
  if (!shapath(sha)) return -1;
  
  FILE *file;
  if (!(file = fopen(INDEX_LOC, "a"))) {
    perror("Error accessing index file");
    return -1;
  }
  char entry[64];
  sprintf(entry, "%s %s\n", sha, path);

  fwrite(entry, sizeof(char), strlen(entry), file);
  printf("Succesfully added blob '%s' to index\n", sha)
  return 0;
}


int flog_add(char *filename) {
  char *sha;
  if (!(sha = make_blob(filename))) {
    fprintf(stderr, "Add operation failed\n");
    return -1;
  } else {
    index_blob(sha, filename);
  }
}
