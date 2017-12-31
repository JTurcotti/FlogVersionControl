//contains low-level functions, not to be interacted with by the user

#include "pproc.h"

//160-bit sha hash to null-terminated string
char *shatos(unsigned char *);

//returns string read from filename
char *read_whole_file(char *filename);

//creates file with contents
int write_whole_fule(char *filename, char *contents);

//returns sha of blob created with contents of file 
char *make_blob(char *filename);

//tests if directory exists in filesystem
int dir_exists(char *path);
