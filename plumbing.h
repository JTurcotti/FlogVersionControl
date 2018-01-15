//contains low-level functions, not to be interacted with by the user

#include "pproc.h"

//convert 160 bit hash from base 256 to base 16
hash_t shatohash(unsigned char *);

//returns hash of object with given type and mode
hash_t hashobj(char *type, char *body);

//returns prospective path
char *shapath(hash_t sha);
char *branchpath(char *branch_name);

//returns string read from filename
char *read_whole_file(char *filename);

//creates file with contents
int write_whole_fule(char *filename, char *contents);

//returns sha of newly created blob created with contents of file 
hash_t make_blob(char *filename);

//returns sha of newly created tree from entries
hash_t make_tree(int n_ent, char ***ent);

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

//restores files from tree into working directory
int tree_build(hash_t tree);

//get info from commit
hash_t get_parent(hash_t commit);
hash_t get_tree(hash_t commit);

//updates index to describe tree with given hash
int tree_build_inex(hash_t tree_sha);
