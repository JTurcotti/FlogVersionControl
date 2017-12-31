all: object_handler.c util.c main.c plumbing.h porcelain.h pproc.h
	gcc -o flog object_handler.c util.c main.c -lssl -lcrypto
