#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include "my_ufs.h"

typedef struct myFILE {
   int fd;
   int flags;
   // int             level;      /* fill/empty level of buffer */
   // unsigned        flags;      /* File status flags          */
   // char            fd;         /* File descriptor            */
   // unsigned char   hold;       /* Ungetc char if no buffer   */
   // int             bsize;      /* Buffer size                */
   // unsigned char   *buffer;    /* Data transfer buffer       */
   // unsigned char   *curp;      /* Current active pointer     */
   // unsigned        istemp;     /* Temporary file indicator   */
   // short           token;      /* Used for validity checking */
} myFILE;

myFILE* myfopen(const char *pathname, const char *mode);
int myfclose(myFILE *stream);
size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream);
size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream);
int myfseek(myFILE *stream, long offset, int whence);
int myfscanf(myFILE *stream, const char *format, ...);
int myprintf(myFILE *stream, const char *format, ...);

