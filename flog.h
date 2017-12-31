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
#define OBJECT_LOC ".flog/objects/"
#define REF_LOC ".flog/refs/"
#define HASH_REPR_LEN 8
#define ALLOC_ERR 16

//initialize empty flog repo
int flog_init();

//160-bit sha hash to null-terminated string
char *shatos(unsigned char *);

//returns string read from filename
char *read_whole_file(char *filename);

//creates filename with contents (rmb to pass hashed name)
int write_whole_fule(char *filename, char *contents);

//returns new blob with name = filename and hash = hash_function of file 
int make_blob(char *filename);

//tests if directory exists in filesystem
int dir_exists(char *path);
