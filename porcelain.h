//contains high-level functions for user interaction

#include "pproc.h"

//initialize empty flog repo
int flog_init();

//add file to index
int flog_add(char *filename);

//create commit from index
int flog_commit(char *msg);
