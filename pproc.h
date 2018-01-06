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

//stores SHA1 160 bit hashes as a hexadecimal string
typedef char* hash_t;

//debugging
#define DEBUG 0

//define structure of .flog data directory
#define MAIN_LOC ".flog"
#define OBJECT_LOC ".flog/objects"
#define REF_LOC ".flog/refs"
#define INDEX_LOC ".flog/index"

//misc constants
#define ALLOC_ERR 16 //prevent malloc errors
#define MAXLN_SIZE 256 //max line size for tracked files
#define MAXPWD_SIZE 64 //max size of a path name
#define MAXIND_SIZE 64 //max size of index
#define MAXTYP_SIZE 8 //max size of obj type

//define format of storage files
#define OBJ_FMT "%s %lu %s" //type, size, body
#define INDEXLN_FMT "%s %s %s\n" //mode, type, hash, path
#define INDEXLN_SCAN "%s %s %s" //alt form for fscanf arg
#define TREELN_FMT "%s %s %s\n" //mode, type, hash, name

//allowable modes for tracked files
#define FILE_MD "100644"
#define EXEC_MD "100755" 
#define DIR_MD  "040000"
#define SYM_MD  "120000" //not yet implemented

//char * return types for signaling (prolly bad practice, though technically an impossible return value of SHA1() anyways...)
#define EBLOB_EXIST "#*&$$%&$%#"
