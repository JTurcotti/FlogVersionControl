#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>

#define OBJECT_LOC ".flog/objects/"
#define HASH_REPR_LEN 8
#define ALLOC_ERR 16

typedef unsigned long hash_t;


struct blob {
  hash_t hash;
  char *name;
};

//trees not yet implemented
struct tree {
  unsigned long hash;
  char *name;
  int num_blobs, num_trees;
  struct blob *blobs;
  struct tree *trees;
};

//returns a representation of a hash_t as string
char *hash_to_string(hash_t hash);

//hashes a character array (non-secure but adequate)
hash_t hash_function(char *str);

//returns string read from filename
char *read_whole_file(char *filename);

//creates filename with contents (rmb to pass hashed name)
int write_whole_fule(char *filename, char *contents);

//returns new blob with name = filename and hash = hash_function of file 
struct blob *make_blob(char *filename);

//tests if directory exists in filesystem
int dir_exists(char *path);
