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


//helper function for index_build to recurse, complicated af
//index: index file; tracked_dir: list of dirs already built; n_dir: number thereof; ent: ent[0] temp storage, rows ent[1...] stores data to build tree; n_dir: number thereof; dirpath: recursive indicator, which dir->tree is currently being built
hash_t index_build_layer(int *n_dir, char **tracked_dir, char *dirpath) {

  int n_ent = 0;
  char ***ent = (char ***) malloc(sizeof(char **) * MAXIND_SIZE);
  int i, j;
  for (i=0; i < MAXIND_SIZE + 1; i++) {
    ent[i] = (char **) malloc(sizeof(char *) * 3);
    for (j=0; j < 3; j++) {
      ent[i][j] = (char *) malloc(sizeof(char) * MAXPWD_SIZE);
    }
  }
  
  
  FILE *index = index_open();
  
  //read each entry into ent[0]: mode hash path
  while (fscanf(index, INDEXLN_SCAN, ent[0][0], ent[0][1], ent[0][2]) != EOF) {
    if (DEBUG) printf("start\n");
    //path adds a leading '/'
    char *path = strcat(strcat(malloc(MAXPWD_SIZE), "/"), ent[0][2]);
    if (DEBUG) printf("initial path: '%s'; ", path);
    char *filename = strrchr(path, '/'); //filename stores everything after last backslash
    *filename++ = '\0';
    //previous value of path = new value + '/' + filename
    if (DEBUG) printf("dirpath: '%s'; path: '%s'; filename: '%s'; n: %d\n", dirpath, path, filename, n_ent);

    //tests if dirpath and path agree 
    if ((!strcmp(path, "") && !strcmp(dirpath, "/")) || !strcmp(path, dirpath)) {
      //file lies in current dirpath, add to ent
      n_ent++;
      ent[n_ent][0] = strdup(ent[0][0]); //mode
      ent[n_ent][1] = strdup(ent[0][1]); //hash
      ent[n_ent][2] = strdup(ent[0][2]); //path
      if (DEBUG) printf("added file %s/%s to %s\n", path, filename, dirpath);
    } else {
      //increment i through tracked_dir until it encounters path
      //note that a gauranteed precondition strcmp(path, dirpath) && (strcmp(path, "") || strcmp(dirpath, "/"))
      
      if (!strstr(path, dirpath)) {
	//if path is not a descesndent of dirpath, continue to next
	continue;
      }
      char *pos = path + strlen(dirpath); //points to '/' after dirpath in path
      while (*pos != '/' && pos - path < strlen(path)) {
	pos++;
      } //points to next '/', or end
      if (*pos == '/') {
	//if pos points to next '/', end path there
	*pos = '\0';
      } //path now contains one dir deeper than dirpath

      int i;
      for (i=0; i < *n_dir; i++) {
	if (!strcmp(path, tracked_dir[i])) {
	  //make sure path not already created
	  break;
	}
      }
      if (i < *n_dir) {
	//if path is already tracked, continue to next path
	continue;
      }

      //add to list of tracked dirs
      tracked_dir[(*n_dir)++] = path;

      //build tree recursively
      if (DEBUG) printf("calling build layer on dirpath %s\n", path);
      hash_t sha = index_build_layer(n_dir, tracked_dir, path);
      n_ent++;
      ent[n_ent][0] = DIR_MD; //mode (always DIR)
      ent[n_ent][1] = strdup(sha); //hash
      ent[n_ent][2] = strdup(path + 1); //path (ignore leading '/')
      if (DEBUG) printf("added dir %s to %s\n", path, dirpath);
    }
  }
  
  if (DEBUG) printf("passing entries to make_tree '%s'\n", dirpath);
  
  hash_t sha = make_tree(n_ent, ent + 1);

  return sha;
}
 

//creates tree from index and returns hash
hash_t index_build() {
  //track data to build tree:
  char **tracked_dir = (char **) malloc(sizeof(char *) * MAXDIR_COUNT);
  int k;
  for (k=0; k < MAXDIR_COUNT; k++) {
    tracked_dir[k] = malloc(sizeof(char) * MAXPWD_SIZE);
  }
  tracked_dir[0] = "/";
  int n_dir = 1;
  
  return index_build_layer(&n_dir, tracked_dir, "/");
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
      printf("File '%s' unchanged in index\n", path);
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
      char cmd[MAXIND_SIZE * 2 + ALLOC_ERR * 10];
      sprintf(cmd, "sed -i -e 's/%s/%s/' %s", line, entry, INDEX_LOC);
      int exit = system(cmd);
      if (!exit && DEBUG) {
	printf("Successfully updated '%s' with blob '%s'\n", path, sha);
      }
      return exit;
    }
    
  }
  
  fwrite(entry, sizeof(char), strlen(entry), index);
  if (DEBUG) printf("Succesfully added blob '%s' to .flog/index\n", sha);
  return 0;
}


//updates index to match tree, return number of entries
int tree_build_index(hash_t tree_sha) {
  char *index_body = calloc(sizeof(char), MAXIND_SIZE * MAXPWD_SIZE);

  int n = tree_build_index_deep(tree_sha, index_body);

  //remove trailing newline
  if (index_body[strlen(index_body) - 1] == '\n')
    index_body[strlen(index_body) - 1] = '\0';
  
  write_whole_file(INDEX_LOC, index_body);

  return n;
}

//recursively creates index entries from tree
int tree_build_index_deep(hash_t tree_sha, char *index_body) {
  char *tree_body = read_whole_file(shapath(tree_sha));

  char mode[8], hash[SHA_DIGEST_LENGTH * 2 + 1], path[MAXPWD_SIZE];
  int n = 0;
  do {
    if (*tree_body == '\n') tree_body++;
    sscanf(tree_body, TREELN_SCAN, mode, hash, path);
    
    if (!strcmp(mode, DIR_MD)) {
      n += tree_build_index_deep(hash, index_body);
    } else {
      sprintf(index_body, "%s" INDEXLN_FMT "\n", strdup(index_body), mode, hash, path);
      n++;
    }
  } while ((tree_body = strchr(tree_body, '\n')) && (strlen(tree_body) > 1)); //while another line left and that line is not a single newline
  return n;
}
