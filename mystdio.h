#ifndef MYSTDIO_H
#define MYSTDIO_H

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "my_ufs.h"

typedef struct myFILE {
   int fd;
   int flags;
} myFILE;

myFILE* myfopen(const char *pathname, const char *mode);
int myfclose(myFILE *stream);
size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream);
size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream);
int myfseek(myFILE *stream, long offset, int whence);
int myfscanf(myFILE *stream, const char *format, ...);
int myfprintf(myFILE *stream, const char *format, ...);

#endif
