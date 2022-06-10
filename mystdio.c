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
        mf->fd = myopen(pathname, O_CREAT);
        mf->flags = O_WRONLY;
    }
    else if(strcmp(mode, "a") == 0) {
        mf->fd = myopen(pathname, O_CREAT | O_WRONLY);
        mf->flags = O_CREAT | O_WRONLY;
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
    if(stream->flags & O_RDONLY) {
        // CONTINUE HERE
    }
}

size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream) {

}

int myfseek(myFILE *stream, long offset, int whence) {

}

int myfscanf(myFILE *stream, const char *format, ...) {

}

int myprintf(myFILE *stream, const char *format, ...) {

}