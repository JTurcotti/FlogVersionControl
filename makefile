all: object_handler.c util.c main.c flog.h
	gcc -o flog object_handler.c util.c main.c -lssl -lcrypto
