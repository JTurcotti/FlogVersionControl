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

//define structure of .flog data directory
#define MAIN_LOC ".flog"
#define OBJECT_LOC ".flog/objects"
#define REF_LOC ".flog/refs"
#define INDEX_LOC ".flog/index"

//misc constants
#define ALLOC_ERR 16 //prevent malloc errors
#define MAXLN_SIZE 256 //max line size for tracked files

//define format of storage files
#define BLOB_FMT "blob %lu %s"
#define INDEXLN_FMT "%s %s\n"

//char * return types for signaling (prolly bad practice, though technically an impossible return value of SHA1() anyways...)
#define EBLOB_EXIST "#*&$$%&$%#"
