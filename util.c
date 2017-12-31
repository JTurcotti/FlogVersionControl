#include "flog.h"

int flog_init() {
  if (dir_exists(MAIN_LOC)) {
    fprintf(stderr, "Flog repo already exists here\n");
  } else if (mkdir(MAIN_LOC, DEFFILEMODE) ||
	     mkdir(OBJECT_LOC, DEFFILEMODE) ||
	     mkdir(REF_LOC, DEFFILEMODE)) {
    perror("Error creating flog repo");
  } else {
    printf("Initialized empty flog repo\n");
    return 0;
  }
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

char *shatos(unsigned char *arr) {
  char *out = calloc(1, SHA_DIGEST_LENGTH * 2 + 1);
  char *itr = out;
  while (itr - out < SHA_DIGEST_LENGTH * 2) {
    itr += sprintf(itr, "%02x", *arr++);
  }
  assert(strlen(out) == SHA_DIGEST_LENGTH * 2);
  return out;
}

char *read_whole_file(char *filename) {
  FILE *file;
  if (!(file = fopen(filename, "r"))) {
    fprintf(stderr, "Error opening input file '%s': %s\n", filename, strerror(errno));
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
  FILE *file;
  if (!(file = fopen(filename, "w"))) {
    fprintf(stderr, "Error creating output file '%s': %s\n", filename, strerror(errno));
  }

  int written = fwrite(contents, sizeof(char), strlen(contents), file);
  fclose(file);

  return written;
}
