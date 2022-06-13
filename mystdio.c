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
        perror("file not found");
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
    perror("cant read in current mode");
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
    perror("cant write in current mode");
    return -1;
}

int myfseek(myFILE *stream, long offset, int whence) {
    if(stream->flags & O_APPEND) {
        return -1;
    }
    return mylseek(stream->fd, offset, whence);
}

int myfscanf(myFILE *stream, const char *format, ...) {
    va_list arg;
    int done = 0;
    size_t read;
    int* i_ptr;
    char* c_ptr;
    float* f_ptr;

    va_start(arg, format);
    
    char* perc = strchr(format, '%');
    while(perc != NULL) {
        switch(*(perc + 1)) {
        case 'd':
            i_ptr = va_arg(arg, int*);
            read = myfread(i_ptr, sizeof(int), 1, stream);
            if(read == -1) {
                return done;
            }
            break;
        case 'c':
            c_ptr = va_arg(arg, char*);
            read = myfread(c_ptr, sizeof(char), 1, stream);
            if(read == -1) {
                return done;
            }
            break;
        case 'f':
            f_ptr = va_arg(arg, float*);
            read = myfread(f_ptr, sizeof(float), 1, stream);
            if(read == -1) {
                return done;
            }
            break;
        default:
            return done;
        }
        done++;
        perc = strchr(perc + 1, '%');
    }
    va_end(arg);
    return done;
}

int myfprintf(myFILE *stream, const char *format, ...) {
    va_list arg;
    int done = 0;
    size_t read;
    int i_val;
    char c_val;
    float f_val;
    char temp[1];

    va_start(arg, format);
    
    for(int i = 0; i < strlen(format); i++) {
        if(*(format + i) == '%') {
            switch(*(format + i + 1)) {
            case 'd':
                i_val = va_arg(arg, int);
                read = myfwrite(&i_val, sizeof(int), 1, stream);
                if(read == -1) {
                    return done;
                }
                break;
            case 'c':
                c_val = va_arg(arg, int);
                read = myfwrite(&c_val, sizeof(char), 1, stream);
                if(read == -1) {
                    return done;
                }
                break;
            case 'f':
                f_val = va_arg(arg, double);
                read = myfwrite(&f_val, sizeof(float), 1, stream);
                if(read == -1) {
                    return done;
                }
                break;
            default:
                return done;
            }
            done++;
            i++;
        }
        else {
            strncpy(temp, format, 1);
            myfwrite(temp, sizeof(char), 1, stream);
            if(read == -1) {
                return done;
            }
        }
    }
    va_end(arg);
    return done;
}
