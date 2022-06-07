#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 10000
#define BLOCK_DATA_SIZE 512
#define NAME_SIZE 8

typedef struct superblock {
    int inodes_amount;
    int blocks_amount;
    int block_size;
} superblock;

typedef struct block {
    char data[BLOCK_DATA_SIZE];
    struct block* next;
    int available;
} block;

typedef struct inode {
    block* first_block;
    char name[NAME_SIZE];
    int size;
} inode;

typedef struct myopenfile {
    inode* my_inode;
    int currser;
} myopenfile;

typedef struct myDIR {
    int idk_yet;
}myDIR;

void mymkfs(int s); // DONE

int mymount(const char* dest); // DONE
// int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
int myopen(const char *pathname, int flags); // DONE
int myclose(int myfd); // DONE
ssize_t myread(int myfd, void *buf, size_t count); // DONE
ssize_t mywrite(int myfd, const void *buf, size_t count); // DONE
off_t mylseek(int myfd, off_t offset, int whence); // DONE
myDIR *myopendir(const char *name);
struct mydirent *myreaddir(myDIR *dirp);
int myclosedir(myDIR *dirp);

void myprint(); // DONE
