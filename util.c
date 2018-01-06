#include "plumbing.h"

int flog_init() {
  if (dir_exists(MAIN_LOC)) {
    fprintf(stderr, "Flog repo already exists here\n");
  } else if (mkdir(MAIN_LOC, DEFFILEMODE) ||
	     mkdir(OBJECT_LOC, DEFFILEMODE) ||
	     mkdir(REF_LOC, DEFFILEMODE)) {
    perror("Error creating flog repo");
    return -1;
  } else {
    printf("Initialized empty flog repo\n");
    return 0;
  }
}

//returns the potential path of an object with the given hash in .flog/objects
char *shapath(hash_t sha) {
  if (DEBUG) printf("finding path for '%s'\n", sha);
  char dir[3];
  strncpy(dir, sha, 2);
  dir[2] = '\0';
  char *path = malloc(MAXPWD_SIZE);
  sprintf(path, "%s/%s/%s", OBJECT_LOC, dir, sha + 2);

  if (DEBUG) printf("found path: '%s'\n", path);
  return path;
}

//nonzero iff directory exists at path
int dir_exists(char *path) {
  DIR *dir = opendir(path);
  if (dir) {
    closedir(dir);
    return 1;
  } else if (ENOENT == errno) {
    return 0;
  } else {
    perror("Error accessing exist dir");
    exit(1);
  }
}

hash_t shatohash(unsigned char *arr) {
  hash_t out = calloc(1, SHA_DIGEST_LENGTH * 2 + 1);
  char *itr = out;
  while (itr - out < SHA_DIGEST_LENGTH * 2) {
    itr += sprintf(itr, "%02x", *arr++);
  }
  assert(strlen(out) == SHA_DIGEST_LENGTH * 2);
  return out;
}

//returns contents of file
char *read_whole_file(char *filename) {
  FILE *file;
  if (!(file = fopen(filename, "r"))) {
    fprintf(stderr, "Error opening input file '%s': %s\n", filename, strerror(errno));
    return NULL;
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

//if file already exists, truncate to 0, otherwise create, then write contents
int write_whole_file(char *filename, char *contents) {
  FILE *file;
  if (!(file = fopen(filename, "w"))) {
    fprintf(stderr, "Error creating output file '%s': %s\n", filename, strerror(errno));
    return -1;
  }

  int written = fwrite(contents, sizeof(char), strlen(contents), file);
  fclose(file);

  return written;
}
