#include "flog.h"

char *hash_to_string(hash_t hash) {
  int len = snprintf(NULL, 0, "%lx", hash);
  char *repr = malloc(len + ALLOC_ERR);
  snprintf(repr, len, "%lx", hash);
  return repr;
}

hash_t hash_function(char *str) {
  hash_t hash = 5381;
  int c;

  while (c = *str++) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

int dir_exists(char *path) {
  DIR *dir = opendir(path);
  if (dir) {
    closedir(dir);
    return 1;
  } else if (ENOENT == errno) {
    return 0;
  } else {
    perror("File system error");
  }
}

char *read_whole_file(char *filename) {
  FILE *file;
  if (!(file = fopen(filename, "r"))) {
    perror("Error opening input file\n");
  }

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  rewind(file);

  char *str = malloc(fsize + ALLOC_ERR);
  fread(str, sizeof(char), fsize, file);
  fclose(file);

  str[fsize] = 0;

  return str;
}

int write_whole_file(char *filename, char *contents) {
  if (!dir_exists(OBJECT_LOC)) {
    perror("Must initialize flog repo before attempting to write\n");
    exit(1);
  }

  char *complete_filename = calloc(sizeof(char), strlen(filename) + strlen(OBJECT_LOC) + ALLOC_ERR);
  strcat(strcat(complete_filename, OBJECT_LOC), filename);

  
  FILE *file;
  if (!(file = fopen(complete_filename, "w"))) {
    perror("Error creating output file\n");
  }

  int written = fwrite(contents, sizeof(char), strlen(contents), file);
  fclose(file);

  return written;
}
