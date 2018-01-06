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
#include <time.h>

//stores SHA1 160 bit hashes as a hexadecimal string
typedef char* hash_t;

//stores info abt an author
typedef struct user_t {
  char *name;
  char *email;
} user_t;

//debugging
#define DEBUG 0

//define structure of .flog data directory
#define MAIN_LOC ".flog"
#define OBJECT_LOC ".flog/objects"
#define REF_LOC ".flog/refs"
#define BRANCH_LOC ".flog/refs/branches"
#define HEAD_LOC ".flog/refs/HEAD"
#define INDEX_LOC ".flog/index"

#define BRANCH_PATH(NAME) BRANCH_LOC "/" #NAME
#define MASTER_BR BRANCH_PATH(master)
//misc constants
#define ALLOC_ERR 16 //prevent malloc errors
#define MAXLN_SIZE 256 //max line size for tracked files
#define MAXPWD_SIZE 64 //max size of a path name
#define MAXIND_SIZE 64 //max size of index line
#define MAXMSG_SIZE 512 //max size of commit message
#define MAXTYP_SIZE 8 //max size of obj type
#define MAXUSR_SIZE 64 //max length of user name and email (each)

//define format of storage files
#define OBJ_FMT "%s %lu %s" //type, size, body
#define INDEXLN_FMT "%s %s %s\n" //mode, type, hash, path
#define INDEXLN_SCAN "%s %s %s" //alt form for fscanf arg
#define TREELN_FMT "%s %s %s\n" //mode, type, hash, name
#define COMMIT_FMT "tree %s\nparent %s\nauthor %s %s %ld\n\n%s" //tree, parent, name, email, time, message

//allowable modes for tracked files
#define FILE_MD "100644"
#define EXEC_MD "100755" 
#define DIR_MD  "040000"
#define SYM_MD  "120000" //not yet implemented

//char * return types for signaling (prolly bad practice, though technically an impossible return value of SHA1() anyways...)
#define EBLOB_EXIST "472860d34f7d0490d0a84832f833f9621ed4beee"
//used as parent for initial commit
#define ROOT_COMMIT "1885e0933b6415fa35e639ddb5da5962b4542f6e"
