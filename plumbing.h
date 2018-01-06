//contains low-level functions, not to be interacted with by the user

#include "pproc.h"

//convert 160 bit hash from base 256 to base 16
hash_t shatohash(unsigned char *);

//returns prospective path for object with given hash
char *shapath(hash_t sha);

//returns string read from filename
char *read_whole_file(char *filename);

//creates file with contents
int write_whole_fule(char *filename, char *contents);

//returns sha of newly created blob created with contents of file 
hash_t make_blob(char *filename);

//returns sha of newly created tree from entries
hash_t make_tree(int n_ent, char *ent[][3]);

//returns sha of newly create commit
hash_t make_commit(hash_t tree, hash_t parent, user_t *author, char *msg);

//returns sha of newly created tree from index
hash_t index_build();

//adds blob to index
int index_addblob(hash_t sha, char *path);

//return zero iff unable to access
int dir_exists(char *path);

//returns sha of most recent commit
hash_t headsha();

//get info from commit
hash_t get_parent(hash_t commit);
hash_t get_tree(hash_t commit);
