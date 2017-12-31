//contains pre-precessor instructions

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <openssl/sha.h>
#include <assert.h>


#define MAIN_LOC ".flog"
#define OBJECT_LOC ".flog/objects"
#define REF_LOC ".flog/refs"
#define INDEX_LOC ".flog/index"
#define ALLOC_ERR 16
