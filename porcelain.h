//contains high-level functions for user interaction

#include "pproc.h"

//initialize empty flog repo
int flog_init();

//add file to index
int flog_add(char *filename);

//create commit from index
int flog_commit(char *msg);

//print log from HEAD back
int flog_log();

//restore commit, branch, or ref
int flog_checkout(char *target);

//create branch
int flog_branch(char *name);

//list branches
int flog_branch_list();
