#include "flog.h"
  
//hashes a file to create a blob tying the filename to hash (used for storage file)
int make_blob(char *filename) {
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
  sprintf(file_path, ".flog/objects/%s/%s", dir, hashed_name + 2);
  sprintf(dir_path, ".flog/objects/%s", dir);

  if (!dir_exists(OBJECT_LOC)) {
    fprintf(stderr, "Must initialize flog repo before attempting to write\n");
    exit(1);
  } else if (mkdir(dir_path, DEFFILEMODE)) {
    fprintf(stderr, "Error creating object-specific dir '%s': %s\n", dir_path, strerror(errno));
  } else {
    write_whole_file(file_path, contents);
    printf("Succesfully wrote blob '%s'\n", hashed_name);
    return 0;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Enter command\n");
    exit(1);
  } else if (!strcmp(argv[1], "init")) {
    flog_init();
  } else {
    make_blob(argv[1]);
  }
  return 0;
    
}
  
