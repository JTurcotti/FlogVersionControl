#include "plumbing.h"

user_t data = {"Rasputin", "GrigoriR@kremlin.gov"};
user_t *global_author = &data;

int flog_add(char *filename) {
  hash_t sha;
  if (!(sha = make_blob(filename))) {
    fprintf(stderr, "Add operation failed\n");
    return -1;
  } else if (!index_addblob(sha, filename)) {
    printf("Added to index blob " ANSI_OBJECT "%s" ANSI_COLOR_RESET "\n", sha);
  }
}

int flog_init() {
  if (dir_exists(MAIN_LOC)) {
    fprintf(stderr, "Flog repo already exists here\n");
  } else if (mkdir(MAIN_LOC, DEFFILEMODE) ||
	     mkdir(OBJECT_LOC, DEFFILEMODE) ||
	     mkdir(REF_LOC, DEFFILEMODE) ||
	     mkdir(TAG_LOC, DEFFILEMODE) ||
	     mkdir(BRANCH_LOC, DEFFILEMODE)) {
    perror("Error creating flog repo");
    return -1;
  } else {

    write_whole_file(MASTER_LOC, ROOT_COMMIT);
    write_whole_file(HEAD_LOC, MASTER_LOC);
    
    printf("Initialized empty flog repo\n");
    return 0;
  }
}

int flog_commit(char *msg) {
  hash_t tree = index_build();
  if (!tree) {
    exit(1);
  }

  hash_t parent_sha = headsha();
  hash_t sha = make_commit(tree, parent_sha, global_author, msg);
  if (strstr(read_whole_file(HEAD_LOC), BRANCH_LOC)) {
    //if HEAD points to a branch, then update that branch
    write_whole_file(read_whole_file(HEAD_LOC), sha);
    char *branch = strrchr(read_whole_file(HEAD_LOC), '/') + 1;
    printf("Successful commit " ANSI_COMMIT "%s" ANSI_COLOR_RESET " in branch " ANSI_OBJECT "%s\n" ANSI_COLOR_RESET, sha, branch);
  } else {
    //detached HEAD
    write_whole_file(HEAD_LOC, sha);
    printf("DETACHED HEAD commit " ANSI_COMMIT "%s\n" ANSI_COLOR_RESET, sha);
  }
  
  return 0;
}

int flog_branch(char *name) {
  if (!access(branchpath(name), F_OK)) {
    printf("Branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET " already exists\n", name);
  } else {
    write_whole_file(branchpath(name), headsha());
    printf("Created branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET "\n", name);
  }
}

int flog_branch_delete(char *name) {
  if (!strcmp(name, "master")) {
    fprintf(stderr, "Cannot delete master branch\n");
    return -1;
  }
  if (access(branchpath(name), F_OK)) {
    printf("Branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET " does not exist\n", name);
  } else if (remove(branchpath(name))) {
    perror("Error removing branch file");
  } else {
    printf("Removed branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET "\n", name);
  }
}
      

int flog_branch_list() {
  DIR *dir = opendir(BRANCH_LOC);
  struct dirent *entry;
  if (!dir) {
    perror("Error accessing branch directory");
    return -1;
  }
  printf("Branches:\n");
  while (entry = readdir(dir)) {
    if (entry->d_type == DT_REG) {
      char *name = entry->d_name;
      if (!access(HEAD_LOC, F_OK) && !strcmp(read_whole_file(HEAD_LOC), branchpath(name))) {
	printf(ANSI_OBJECT "* %s\n" ANSI_COLOR_RESET, name);
      } else {
	printf("  %s\n", name);
      }
    }
  }
}
      
  
int flog_log() {
  hash_t sha = headsha();
  char *commit_body, *output1, *output2;
  output1 = malloc(MAXLOG_SIZE);
  output2 = malloc(MAXLOG_SIZE);
  while (strcmp(sha, ROOT_COMMIT)) {
    sprintf(output2, "Commit " ANSI_COMMIT "%s" ANSI_COLOR_RESET ":\n\n%s\n" HORIZ_RULE "\n\n", sha, read_whole_file(shapath(sha)));
    strcat(output2, output1);
    strcpy(output1, output2);
    
    sha = get_parent(sha);
  }

  printf(ANSI_TITLE "Commit History" ANSI_COLOR_RESET ":\n\n%s", output1);
}

//accepts branch name returns number of files checked out
int flog_checkout(char *target) {
  char *commit_sha;
  
  if (!strcmp(target, "HEAD")) {
    commit_sha = headsha();
  } else if (!access(branchpath(target), F_OK)) {
    //target is a branch name
    commit_sha = read_whole_file(branchpath(target)); //get tip
    write_whole_file(HEAD_LOC, branchpath(target));//move HEAD to tip
    printf("Switched to branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET "\n", target);
  } else if (!access(shapath(target), F_OK)) {
    //target is a commit hash
    commit_sha = target; 
    write_whole_file(HEAD_LOC, target); //move HEAD directly to commit
    printf("Checked out commit to " ANSI_COMMIT "%s" ANSI_COLOR_RESET ", HEAD is now detached\n", commit_sha);
  } else {
    fprintf(stderr, "Target '%s' does not match any branch or commit\n", target);
    return -1;
  }

  //if HEAD points to a commit (not root), then build and update index
  if (strcmp(commit_sha, ROOT_COMMIT)) {
    tree_build_index(get_tree(commit_sha));
    return tree_build(get_tree(commit_sha));
  }
}

int flog_reset(char *target, int level) {
  if (!strcmp(target, "HEAD"))
    target = headsha();
  else if (!access(branchpath(target), F_OK)) {
    //target is branch name
    target = read_whole_file(branchpath(target));
  }
  
  if (level >= 0) {
    if (access(shapath(target), R_OK)) {
      //target innaccessible
      fprintf(stderr, "Target " ANSI_COMMIT "%s" ANSI_COLOR_RESET " is not a valid commit\n", target);
    }
    
    if (strstr(read_whole_file(HEAD_LOC), BRANCH_LOC)) {
      //HEAD points to branch (attached)
      write_whole_file(read_whole_file(HEAD_LOC), target); //move branch to target
      
      char *branch_name = strrchr(read_whole_file(HEAD_LOC), '/') + 1;
      
      printf("Branch " ANSI_OBJECT "%s" ANSI_COLOR_RESET " reset to " ANSI_COMMIT "%s\n" ANSI_COLOR_RESET, branch_name, target);
    } else {
      //detached HEAD
      write_whole_file(HEAD_LOC, target);
      
      printf("HEAD reset to " ANSI_COMMIT "%s\n" ANSI_COLOR_RESET, target);
    }
  }

  if (level >= 1) {
    tree_build_index(get_tree(headsha()));
    printf("Index restored to " ANSI_COMMIT "%s\n" ANSI_COLOR_RESET, headsha());
  }
  if (level >= 2) {
    tree_build(get_tree(headsha()));
    printf("Working directory restored to " ANSI_COMMIT "%s\n" ANSI_COLOR_RESET, headsha());
  }
  return 0;
}
