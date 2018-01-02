#include "plumbing.h"


//writes obj to .flog/objects with given type and body, called from all make_<type>, returns hash of obj or EBLOB_EXIST if already exists
hash_t make_obj(char *type, char *body) {
  size_t s_body = strlen(body);
  //holds header and body:
  char obj[s_body + MAXPWD_SIZE];
  sprintf(obj, OBJ_FMT, type, s_body, body);
  
  //hashes the generated obj and converts string from base 256 to base 16
  hash_t sha = shatohash(SHA1(obj, strlen(obj), NULL));
  if (!access(shapath(sha), F_OK)) {
    //blob already exists
    return EBLOB_EXIST;
  }
  
  //stores first two charactes of hash as dir name, 
  char dir[3], dir_path[MAXPWD_SIZE];
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
    printf("Succesfully wrote blob '%s' to .flog/objects/\n", sha);
    return sha;
  }
}
    
//entries has dimensions *char[num_entires][3], where for any i<num_entries char[i] = {char *mode, char *sha, char *name}
hash_t make_tree(int n_ent, char ***ent) {
  //INIT BODY
  char body[(SHA_DIGEST_LENGTH * 2 + ALLOC_ERR)* n_ent]; //accts for null-terminators and newlines

  int i = -1;
  while (++i < n_ent) {
    char ent_i[SHA_DIGEST_LENGTH * 2];
    sprintf(ent_i, TREELN_FMT, ent[i][0], ent[i][1], ent[i][2]);
    strcat(body, ent_i);
  }

  return make_obj("tree", body);
}

//write blob given file whose contents it holds
hash_t make_blob(char *filename) { 
  return make_obj("blob", read_whole_file(filename));
}

int flog_add(char *filename) {
  hash_t sha;
  if (!(sha = make_blob(filename))) {
    fprintf(stderr, "Add operation failed\n");
    return -1;
  } else if (sha == EBLOB_EXIST) {
    fprintf(stderr, "File '%s' matches preexisting blob; no changes to add\n", filename);
  } else {
    index_addblob(sha, filename);
  }
}
