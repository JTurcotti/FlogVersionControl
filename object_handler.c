#include "plumbing.h"
  
//hashes a file to create a blob tying the filename to hash (used for storage file)
char *make_blob(char *filename) {
  char *contents = read_whole_file(filename);
  
  size_t n = strlen(contents);
  char str[n + 32];
  sprintf(str, BLOB_FMT, n, contents);

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
    // -> no flog repo init
    fprintf(stderr, "Must initialize flog repo before attempting to write\n");
    return NULL;
  } else if (mkdir(dir_path, DEFFILEMODE)) {
    //cannot create dir
    if (errno == EEXIST) {
      //cannot create dir bc already exists, not an error state
      return EBLOB_EXIST;
    } else {
      //cannot create dir bc of a REAL error
      fprintf(stderr, "Error creating object-specific dir '%s': %s\n", dir_path, strerror(errno));
      return NULL;
    }
  } else {
    //everything went perfectly :)
    write_whole_file(file_path, contents);
    printf("Succesfully wrote blob '%s' to .flog/objects/\n", hashed_name);
    return hashed_name;
  }
}

//indexes a blob called sha representing a file at path and returns 0 if not already present, 1 if already present (this is NOT an error case), or fails and returns -1
int index_blob(char *sha, char *path) {
  //make sure there's actually a blob
  if (!shapath(sha)) {
    fprintf(stderr, "Read Error: blob '%s' does not exist\n", sha);
    return -1;
  }

  //format entry before writing
  char entry[64];
  sprintf(entry, INDEXLN_FMT, sha, path);
  
  FILE *file;
  if (!(file = fopen(INDEX_LOC, "r+"))) {
    if (errno == ENOENT) {
      file = fopen(INDEX_LOC, "w+");
    } else {
      perror("Error accessing index file");
      return -1;
    }
  }
  
  char line[MAXLN_SIZE];
  //check each existant line against entry
  while (fgets(line, MAXLN_SIZE, file)) {
    if (!strcmp(line, entry)) {
      //this should never happen unless someone deletes object files manually
      printf("Blob '%s' already present in .flog/index\n", sha);
      return 1;
    }
  }
  
  fwrite(entry, sizeof(char), strlen(entry), file);
  printf("Succesfully added blob '%s' to .flog/index\n", sha);
  return 0;
}


int flog_add(char *filename) {
  char *sha;
  if (!(sha = make_blob(filename))) {
    fprintf(stderr, "Add operation failed\n");
    return -1;
  } else if (sha == EBLOB_EXIST) {
    fprintf(stderr, "File '%s' matches preexisting blob; no changes to add\n", filename);
  } else {
    index_blob(sha, filename);
  }
}
