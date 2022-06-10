#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 10000
#define BLOCK_DATA_SIZE 512
#define NAME_SIZE 24

typedef struct mydirent {
    int inode_num;
    char d_name[NAME_SIZE];
} mydirent;

typedef struct superblock {
    int inodes_amount;
    int blocks_amount;
    int block_size;
    mydirent root;
} superblock;

typedef struct block {
    char data[BLOCK_DATA_SIZE];
    int next;
    int available;
} block;

typedef struct inode {
    int first_block;
    int type;
    int blocks_amount;
    int mydirent_amount;
} inode;

typedef struct myopenfile {
    int my_inode;
    int curser;
} myopenfile;

typedef struct myDIR {
    int inode;
    int curser;
} myDIR;

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
int allocate_file_folder(int type); // DONE
void attach_to_folder(myDIR* mdir, int ino, const char* filename); // DONE
void free_mem();
