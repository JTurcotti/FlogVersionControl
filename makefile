all: blob.c util.c main.c flog.h
	gcc -o flog blob.c util.c main.c -lssl -lcrypto
