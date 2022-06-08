#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 10000
#define BLOCK_DATA_SIZE 512
#define NAME_SIZE 16

typedef struct superblock {
    int inodes_amount;
    int blocks_amount;
    int block_size;
} superblock;

typedef struct block {
    char data[BLOCK_DATA_SIZE];
    int next;
    int available;
} block;

typedef struct inode {
    int first_block;
    int type; // -1 for init, 0 for file, 1 for folder
    int size;
    int amount;
} inode;

typedef struct myopenfile {
    int my_inode;
    int currser;
} myopenfile;

typedef struct myDIR {
    int inode;
    int currser;
} myDIR;

typedef struct mydirent {
    int inode_num;
    char d_name[NAME_SIZE];
} mydirent;

void mymkfs(int s); // DONE

void save_to_file(); // DONE
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
int myopen(const char *pathname, int flags); // DONE
int myclose(int myfd); // DONE
ssize_t myread(int myfd, void *buf, size_t count); // DONE
ssize_t mywrite(int myfd, const void *buf, size_t count); // DONE
off_t mylseek(int myfd, off_t offset, int whence); // DONE
myDIR *myopendir(const char *name);
struct mydirent *myreaddir(myDIR *dirp);
int myclosedir(myDIR *dirp);

void myprint(); // DONE
int allocate_file_folder(const char *pathname, int type); // DONE
void attach_file_to_folder(myDIR* mdir, int ino, const char* filename); // DONE
void free_mem();
