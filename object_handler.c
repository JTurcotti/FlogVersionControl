#include "plumbing.h"


//writes obj to .flog/objects with given type and body, called from all make_<type>, returns hash of obj or EBLOB_EXIST if already exists
hash_t make_obj(char *type, char *body) {
  if (DEBUG) printf("Making %s with body: '%s'\n", type, body);
  
  
  size_t s_body = strlen(body);
  //holds header and body:
  char obj[s_body + MAXPWD_SIZE];
  sprintf(obj, OBJ_FMT, type, s_body, body);

  if (DEBUG) printf("Obj content generated: '%s'\n", obj);
  
  //hashes the generated obj and converts string from base 256 to base 16
  hash_t sha = shatohash(SHA1(obj, strlen(obj), NULL));

  if (DEBUG) printf("Obj successfully hashed: '%s'\n", sha);
							 
  if (!access(shapath(sha), F_OK)) {
    //blob already exists
    if (DEBUG) printf("Obj already exists, aborting\n");
    return EBLOB_EXIST;
  }

  //stores first two charactes of hash as dir name, 
  char *dir = malloc(3);
  char *dir_path = malloc(strlen(OBJECT_LOC) + 5);
  strncpy(dir, sha, 2);
  dir[2] = '\0';
  sprintf(dir_path, "%s/%s", OBJECT_LOC, dir);
  
  if (!dir_exists(OBJECT_LOC)) {
    // -> no flog repo init
    fprintf(stderr, "Must initialize flog repo before attempting to write\n");
    return NULL;
  } else if (mkdir(dir_path, DEFFILEMODE) && errno != EEXIST) {
    //unable to make dir AND dir does not already exist
    fprintf(stderr, "Error creating object-specific dir '%s': %s\n", dir_path, strerror(errno));
    return NULL;
  } else {
    write_whole_file(shapath(sha), body);
    if (DEBUG) printf("Succesfully wrote %s '%s' to .flog/objects/\n", type, sha);
    return sha;
  }
}
    
//entries has dimensions *char[num_entries][3], where for any i<num_entries char[i] = {char *mode, char *sha, char *name}
hash_t make_tree(int n_ent, char *ent[][3]) {
  //INIT BODY
  if (DEBUG) printf("constructing tree\n");
  char *body = calloc((SHA_DIGEST_LENGTH + MAXPWD_SIZE + MAXTYP_SIZE + ALLOC_ERR), n_ent); //accts for null-terminators and newlines

  int i = -1;
  while (++i < n_ent) {
    char ent_i[SHA_DIGEST_LENGTH + MAXPWD_SIZE + MAXTYP_SIZE + ALLOC_ERR];
    sprintf(ent_i, TREELN_FMT, ent[i][0], ent[i][1], ent[i][2]);
    strcat(body, ent_i);
  }

  if (DEBUG) printf("within tree: ready to pass body '%s'\n", body);
  
  return make_obj("tree", body);
}

//write blob given file whose contents it holds
hash_t make_blob(char *filename) { 
  return make_obj("blob", read_whole_file(filename));
}

hash_t make_commit(hash_t tree, hash_t parent, user_t *author, char *msg) {
  char *body = malloc(SHA_DIGEST_LENGTH * 2 + MAXUSR_SIZE + MAXMSG_SIZE + ALLOC_ERR);

  time_t timer = time(NULL);
  sprintf(body, COMMIT_FMT, tree, parent, author->name, author->email, asctime(localtime(&timer)), msg);

  return make_obj("commit", body);
}


//creates/restores all files from tree and returns number thereof  
int tree_build(hash_t tree_sha) {
  char *tree_body = read_whole_file(shapath(tree_sha));
  char mode[8], hash[SHA_DIGEST_LENGTH * 2 + 1], path[MAXPWD_SIZE];
  int n = 0;
  do {
    sscanf(tree_body, TREELN_SCAN, mode, hash, path);
    write_whole_file(path, read_whole_file(shapath(hash)));
    n++;
  } while ((tree_body = strchr(tree_body, '\n')) && (strlen(tree_body) > 1));
  return n;
}
