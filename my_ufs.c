#include "my_ufs.h"

superblock sb;
inode* inodes;
block* blocks;
mydirent root;
myopenfile fds[MAX_FILES];
int avail_fd = 3;

void mymkfs(int s) {
    sb.inodes_amount = (s / sizeof(inode)) / 10;
    sb.blocks_amount = (s - (sb.inodes_amount * sizeof(inode)) - sizeof(superblock)) / sizeof(block);
    sb.block_size = sizeof(block);

    printf("inodes: %d\n", sb.inodes_amount);
    printf("blocks: %d\n", sb.blocks_amount);
    printf("size of superblock: %ld\n", sizeof(superblock));
    printf("size of inode: %ld\n", sizeof(inode));
    printf("size of block: %ld\n", sizeof(block));

    inodes = (inode*)malloc(sizeof(inode) * sb.inodes_amount);
    if(inodes == NULL) {
        perror("no memory");
        exit(0);
    }
    for(int i = 0; i < sb.inodes_amount; i++) {
        inodes[i].first_block = -1;
        // strcpy(inodes[i].name, "-------");
        inodes[i].type = -1;
        inodes[i].size = 0;
    }

    blocks = (block*)malloc(sizeof(block) * sb.blocks_amount);
    if(blocks == NULL) {
        perror("no memory");
        exit(0);
    }
    for(int i = 0; i < sb.blocks_amount; i++) {
        blocks[i].next = -1;
        blocks[i].available = 1;
    }

    strcpy(root.d_name, "/");
    root.inode_num = allocate_file_folder("/", 1);

    save_to_file();
}

void free_mem() {
    free(inodes);
    free(blocks);
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
    FILE* fp;
    fp = fopen(source, "r");
    if(fp == NULL) {
        perror("fopen");
        return -1;
    }
    fread(&sb, sizeof(superblock), 1, fp);
    fread(inodes, sizeof(inode), sb.inodes_amount, fp);
    fread(blocks, sizeof(block), sb.blocks_amount, fp);    
    fclose(fp);
    return 0;
}

void save_to_file() {
    FILE* fp;
    fp = fopen("myFile.txt", "w+");
    if(fp == NULL) {
        perror("fopen");
        return;
    }
    fwrite(&sb, sizeof(superblock), 1, fp);
    fwrite(inodes, sizeof(inode), sb.inodes_amount, fp);
    fwrite(blocks, sizeof(block), sb.blocks_amount, fp);    
    fclose(fp);
}

void myprint() {
    printf("superblock info:\n");
    printf("\tinodes amount: %d\n", sb.inodes_amount);
    printf("\tblocks amount: %d\n", sb.blocks_amount);
    printf("\tblock size: %d\n", sb.block_size);

    printf("inodes:\n");
    for(int i = 0; i < sb.inodes_amount; i++) {
        printf("\tinode num: %d size: %d block: %d\n", i, inodes[i].size, inodes[i].first_block);
    }
    printf("blocks:\n");
    for(int i = 0; i < sb.blocks_amount; i++) {
        printf("\tblock num: %d next block: %d available: %d\n", i, blocks[i].next, blocks[i].available);
    }
}

int find_available_inode() {
    for(int i = 0; i < sb.inodes_amount; i++) {
        if(inodes[i].first_block == -1) {
            return i;
        }
    }
    return -1;
}

int find_available_block() {
    for(int i = 0; i < sb.blocks_amount; i++) {
        if(blocks[i].available == 1) {
            return i;
        }
    }
    return -1;
}

int allocate_file_folder(const char *pathname, int type) {
    int avail_inode = find_available_inode();
    if(avail_inode == -1) {
        return -1;
    }
    int avail_block = find_available_block();
    if(avail_block == -1) {
        return -1;
    }
    inodes[avail_inode].first_block = avail_block;
    inodes[avail_inode].size = 1;
    inodes[avail_inode].type = type;
    blocks[avail_block].available = 0;
    // strcpy(inodes[avail_inode].name, pathname);
    return avail_inode;
}

int myopen(const char *pathname, int flags) {

    // strtok(pathname, "/");
    // char* path = "/";
    // for(int i = 0; i < sb.inodes_amount; i++) {
    //     if(strcmp(path, inodes[i].name) == 0) {
    //         DIR *hey;
    //     }
    // }

    for(int i = 0; i < sb.inodes_amount; i++) {
        if(strcmp(pathname, inodes[i].name) == 0) {
            fds[avail_fd].my_inode = i;
            fds[avail_fd].currser = 0;
            return avail_fd++;
        }
    }
    int new_inode = allocate_file_folder(pathname, 0);
    fds[avail_fd].my_inode = new_inode;
    fds[avail_fd].currser = 0;
    return avail_fd++; 
}

int myclose(int myfd) {
    if(myfd >= avail_fd) {
        return -1;
    }
    fds[myfd].my_inode = -1;
    return 0;
}

ssize_t myread(int myfd, void *buf, size_t count) {
    if(fds[myfd].my_inode == -1) {
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].currser / BLOCK_DATA_SIZE;
    if(block_num >= inodes[fds[myfd].my_inode].size) {
        return -1;
    }
    int curr_block = inodes[fds[myfd].my_inode].first_block;
    for(int i = 0; i < count; i++) {
        if(fds[myfd].currser != 0 && fds[myfd].currser % BLOCK_DATA_SIZE == 0) {
            if(blocks[curr_block].next == -1) {
                return i;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = fds[myfd].currser % BLOCK_DATA_SIZE;
        text[i] = blocks[curr_block].data[pos];
        fds[myfd].currser++;
    }
    return count;
}

ssize_t mywrite(int myfd, const void *buf, size_t count) {
    if(fds[myfd].my_inode == -1) {
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].currser / BLOCK_DATA_SIZE;
    int curr_block = inodes[fds[myfd].my_inode].first_block;
    for(int i = 0; i < block_num; i++) {
        if(blocks[curr_block].next == -1) {
            int avail_block = find_available_block();
            if(avail_block == -1) {
                return -1;
            }
            blocks[avail_block].available = 0;
            blocks[curr_block].next = avail_block;
            inodes[fds[myfd].my_inode].size++;
        }
        curr_block = blocks[curr_block].next;
    }
    for(int i = 0; i < count; i++) {
        if(fds[myfd].currser != 0 && (fds[myfd].currser % BLOCK_DATA_SIZE) == 0) {
            if(blocks[curr_block].next == -1) {
                int avail_block = find_available_block();
                if(avail_block == -1) {
                    return -1;
                }
                blocks[avail_block].available = 0;
                blocks[curr_block].next = avail_block;
                inodes[fds[myfd].my_inode].size++;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = fds[myfd].currser % BLOCK_DATA_SIZE;
        blocks[curr_block].data[pos] = text[i];
        fds[myfd].currser++;
    }
    save_to_file();
    return count;
}

off_t mylseek(int myfd, off_t offset, int whence) {
    if(whence == SEEK_SET) {
        fds[myfd].currser = offset;
        return fds[myfd].currser;
    }
    else if(whence == SEEK_CUR) {
        fds[myfd].currser += offset;
        return fds[myfd].currser;
    }
    else if(whence == SEEK_END) {
        fds[myfd].currser = inodes[fds[myfd].my_inode].size * BLOCK_DATA_SIZE - offset;
        return fds[myfd].currser;
    }
    return -1;
}

myDIR* myopendir(const char *name) {
    myopenfile mfd;
    mfd.my_inode = root.inode_num;
    mfd.currser = 0;
    
    myDIR* md = (myDIR*)malloc(sizeof(myDIR));
    if(md == NULL) {
        perror("no memory");
        exit(0);
    }
    // md->inode = 
    // CONTINUE HERE WITH INITIALIZING MD
}

struct mydirent *myreaddir(myDIR *dirp) { // NOT DONE, WHAT TO RETURN?
    char buff[sizeof(mydirent)];
    int block_num = dirp->currser / BLOCK_DATA_SIZE;
    if(block_num >= inodes[dirp->inode].size) {
        return -1;
    }
    int curr_block = inodes[dirp->inode].first_block;
    for(int i = 0; i < sizeof(mydirent); i++) {
        if(dirp->currser != 0 && dirp->currser % BLOCK_DATA_SIZE == 0) {
            if(blocks[curr_block].next == -1) {
                return i;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = dirp->currser % BLOCK_DATA_SIZE;
        buff[i] = blocks[curr_block].data[pos];
        dirp->currser++;
    }
}

int myclosedir(myDIR *dirp) { // NOT DONE
    free(dirp);
}
