#include "flog.h"
  
//hashes a file to create a blob tying the filename to hash (used for storage file)
struct blob *make_blob(char *filename) {
  char *str = read_whole_file(filename);
  char *hashed_name = hash_to_string(hash_function(str));
  
  write_whole_file(hashed_name, str);
  struct blob *new_blob;
  
  new_blob = malloc(sizeof(struct blob) + ALLOC_ERR);
  new_blob->name = filename;
  new_blob->hash = hash_function(str);


  return new_blob;
}

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  struct blob *blob = make_blob(filename);
  printf("Blob '%s' created with hash '%s'\n", blob->name, hash_to_string(blob->hash));
  return 0;
}
  
