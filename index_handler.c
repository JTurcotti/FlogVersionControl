#include "plumbing.h"

FILE *index_open() {
  FILE *index;
  if (!(index = fopen(INDEX_LOC, "r+"))) {
    if (errno == ENOENT) {
      return fopen(INDEX_LOC, "w+");
    } else {
      perror("Error accessing existant index file");
      return NULL;
    }
  } else {
    return index;
  }
}


//creates tree from index and returns hash
hash_t index_build() {
  //track data to build tree:
  int n_ent = 0;
  char *ent[MAXIND_SIZE][3];
  //recieving addresses for fscanf:
  char mode[8], hash[SHA_DIGEST_LENGTH * 2 + 1], path[MAXPWD_SIZE];

  FILE *index = index_open();
  while (fscanf(index, INDEXLN_SCAN, mode, hash, path) != EOF) {

    if (!strchr(path, '/')) {
      ent[n_ent][0] = strdup(mode);
      ent[n_ent][1] = strdup(hash);
      ent[n_ent][2] = strdup(path);
      n_ent++;
    } else {
      //file in subdir
    }
  }

  if (DEBUG) printf("passing entries to make_tree\n");

  hash_t sha = make_tree(n_ent, ent);
  if (!strcmp(sha, EBLOB_EXIST)) {
    //no changes made to index since last write
    printf("No staged changes to commit\n");
    return NULL;
  } else {
    return sha;
  }
}   

//indexes a blob called sha representing a file at path and returns 0 if not already present, 1 if already present (this is NOT an error case), or fails and returns -1
int index_addblob(hash_t sha, char *path) {
  //make sure there's actually a blob
  if (access(shapath(sha), F_OK)) {
    fprintf(stderr, "Read Error: blob '%s' nonexistant\n", sha);
    return -1;
  }

  //determine permissions of file for current user
  char *mode;
  if (access(path, R_OK)) {
    fprintf(stderr, "Blob '%s' references unreadable file\n", sha);
  } else if (access(path, X_OK)) {
    //non-executable
    mode = FILE_MD;
  } else {
    //executable
    mode = EXEC_MD;
  }
  
  //format entry before writing
  char entry[MAXLN_SIZE];
  sprintf(entry, INDEXLN_FMT, mode, sha, path);

  //get index file
  FILE *index = index_open();
  
  char line[MAXLN_SIZE];
  //check each existant line against entry
  while (fgets(line, MAXLN_SIZE, index)) {
    if (!strcmp(line, entry)) {
      //this should never happen unless someone deletes object files manually
      printf("Blob '%s' already present in .flog/index\n", sha);
      return 1;
    }

    //check if an object representing a file at path already exists
    char *line_path;
    if (!strtok(strdup(line), " \n") || !strtok(NULL, " \n") || !(line_path = strtok(NULL, " \n"))) {
      //strtok returns !=0
      perror("Error parsing index line");
    } else if (!strcmp(path, line_path)) {
      //eliminate newlines
      line[strlen(line) - 1] = '\0';
      entry[strlen(entry)-1] = '\0';

      //uses gnu sed to alter index file (cheating???)
      char *cmd;
      sprintf(cmd, "sed -i -e 's/%s/%s/' %s", line, entry, INDEX_LOC);
      int exit = system(cmd);
      if (!exit) {
	printf("Successfully updated '%s' with blob '%s'\n", path, sha);
      }
      return exit;
    }
    
  }
  
  fwrite(entry, sizeof(char), strlen(entry), index);
  printf("Succesfully added blob '%s' to .flog/index\n", sha);
  return 0;
}
