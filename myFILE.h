#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

typedef struct myFILE {
   int             level;      /* fill/empty level of buffer */
   unsigned        flags;      /* File status flags          */
   char            fd;         /* File descriptor            */
   unsigned char   hold;       /* Ungetc char if no buffer   */
   int             bsize;      /* Buffer size                */
   unsigned char   *buffer;    /* Data transfer buffer       */
   unsigned char   *curp;      /* Current active pointer     */
   unsigned        istemp;     /* Temporary file indicator   */
   short           token;      /* Used for validity checking */
}myFILE;

myFILE* myfopen(const char *pathname, const char *mode); //restrict?
int myfclose(myFILE *stream);
size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream); //restrict?
size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream); //restrict?
int myfseek(myFILE *stream, long offset, int whence); //restrict?
int myfscanf(myFILE *stream, const char *format, ...); //restrict?
int myprintf(myFILE *stream, const char *format, ...);

