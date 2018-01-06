all: object_handler.c index_handler.c flog_usr.c util.c main.c plumbing.h porcelain.h pproc.h
	gcc -o flog object_handler.c index_handler.c flog_usr.c util.c main.c -lssl -lcrypto -fno-stack-protector 
