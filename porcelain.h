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

//move head, update, index, and change working directory
int flog_checkout(char *target);

//(0: soft) move branch tip, (1: mixed) update index, (2: hard) change working dir
int flog_reset(char *target, int level);

//create branch
int flog_branch(char *name);

//delete a branch
int flog_branch_delete(char *name);

//list branches
int flog_branch_list();
