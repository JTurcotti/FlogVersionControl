#include "plumbing.h"


//writes obj to .flog/objects with given type and body, called from all make_<type>, returns hash of obj or EBLOB_EXIST if already exists
hash_t make_obj(char *type, char *body) {
  if (DEBUG) printf("Making %s with body: '%s'\n", type, body);

  hash_t sha = hashobj(type, body);

  if (!access(shapath(sha), F_OK)) {
    //object already exists, we're done!
    return sha;
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
hash_t make_tree(int n_ent, char ***ent) {
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


//recursively creates/restores all files from tree and returns number thereof, 
int tree_build(hash_t tree_sha) {
  printf("Starting to build tree %s\n", tree_sha);
  char *tree_body = read_whole_file(shapath(tree_sha));
  printf("read body\n");
  char mode[8], hash[SHA_DIGEST_LENGTH * 2 + 1], path[MAXPWD_SIZE];
  int n = 0;
  do {
    if (*tree_body == '\n') tree_body++; //account for leadding newlines
    //read line of tree (recurse-level-specific file pointer)
    sscanf(tree_body, TREELN_SCAN, mode, hash, path);

    if (!strcmp(mode, DIR_MD)) {
      //lines points to subdir, recursively constructed
      n += tree_build(hash);
    } else {
      printf("Creating %s from hash %s, mode %s\nbody: '%s'\n", path, hash, mode, tree_body);
      write_whole_file(path, read_whole_file(shapath(hash)));
      printf("created!\n");
      n++;
    }
  } while ((tree_body = strchr(tree_body, '\n')) && (strlen(tree_body) > 1)); //while another line left and that line is not a single newline
  return n;
}
