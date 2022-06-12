#include "mystdio.h"

myFILE* myfopen(const char *pathname, const char *mode) {
    myFILE* mf = (myFILE*)malloc(sizeof(myFILE));
    if(mf == NULL) {
        perror("no memory");
        return NULL;
    }
    if(strcmp(mode, "r") == 0) {
        mf->fd = myopen(pathname, 0);
        mf->flags = O_RDONLY;
    }
    else if(strcmp(mode, "r+") == 0) {
        mf->fd = myopen(pathname, 0);
        mf->flags = O_RDWR;
    }
    else if(strcmp(mode, "w") == 0) {
        mf->fd = myopen(pathname, O_WRONLY | O_CREAT | O_TRUNC);
        mf->flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if(strcmp(mode, "a") == 0) {
        mf->fd = myopen(pathname, O_WRONLY | O_CREAT | O_APPEND);
        mf->flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    if(mf->fd == -1) {
        free(mf);
        return NULL;
    }
    return mf;
}

int myfclose(myFILE *stream) {
    if(stream == NULL) {
        return -1;
    }
    myclose(stream->fd);
    free(stream);
    return 0;
}

size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream) {
    if((stream->flags & O_RDONLY) == 0 || stream->flags & O_RDWR) {
        size_t count = 0, read_now;
        for(size_t i = 0; i < nmemb; i++) {
            read_now = myread(stream->fd, (char*)ptr + (i * size), size);
            if(read_now <= 0) {
                return count;
            }
            count += read_now;
        }
        return count;
    }
    return -1;
}

size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream) {
    if(stream->flags & O_WRONLY || stream->flags & O_RDWR) {
        size_t count = 0, wrote;
        for(int i = 0; i < nmemb; i++) {
            wrote = mywrite(stream->fd, (char*)ptr + (i * size), size);
            if(wrote <= 0) {
                return count;
            }
            count += wrote;
        }
        return count;
    }
    return -1;
}

int myfseek(myFILE *stream, long offset, int whence) {
    if(stream->flags & O_APPEND) {
        return -1;
    }
    return mylseek(stream->fd, offset, whence);
}

int myfscanf(myFILE *stream, const char *format, ...) {
    // va_list arg;
    // int done;

    // va_start (arg, format);
    // printf("arg-> :%s: format-? :%s:\n", arg, format);
    // // done = __vfscanf (stream, format, arg);
    // // va_end (arg);

    // return done;
}

int myprintf(myFILE *stream, const char *format, ...) {

}
