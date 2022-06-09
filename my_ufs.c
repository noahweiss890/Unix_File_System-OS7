#include "my_ufs.h"

superblock sb;
inode* inodes;
block* blocks;

// mydirent root;
// myDIR* root;
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
        inodes[i].type = -1;
        inodes[i].blocks_amount = 0;
        inodes[i].mydirent_amount = 0;
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
    strcpy(sb.root.d_name, "/");
    sb.root.inode_num = allocate_file_folder(1);
    save_to_file();
}

void free_mem() {
    free(inodes);
    free(blocks);
    printf("freed\n");
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
    printf("size of superblock: %ld\n", sizeof(superblock));
    printf("size of inode: %ld\n", sizeof(inode));
    printf("size of block: %ld\n", sizeof(block));

    FILE* fp;
    fp = fopen(source, "r");
    if(fp == NULL) {
        perror("fopen");
        return -1;
    }
    fread(&sb, sizeof(superblock), 1, fp);
    printf("inodes: %d\n", sb.inodes_amount);
    printf("blocks: %d\n", sb.blocks_amount);
    inodes = (inode*)malloc(sizeof(inode) * sb.inodes_amount);
    if(inodes == NULL) {
        perror("no memory");
        exit(0);
    }
    blocks = (block*)malloc(sizeof(block) * sb.blocks_amount);
    if(blocks == NULL) {
        perror("no memory");
        exit(0);
    }
    fread(&inodes, sizeof(inode), sb.inodes_amount, fp);
    fread(&blocks, sizeof(block), sb.blocks_amount, fp);    
    fclose(fp);
    // printf("sb info -> inodes amount: %d blocks amount: %d\n", sb.inodes_amount, sb.blocks_amount);
    
    printf("bla: %p\n", inodes[0].type); // THIS ISNT WORKING CUZ SEGMENTATION FAULT
    
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
        printf("\tinode num: %d size: %d first block: %d amount: %d type: %d\n", i, inodes[i].blocks_amount, inodes[i].first_block, inodes[i].mydirent_amount, inodes[i].type);
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

int allocate_file_folder(int type) {
    int avail_inode = find_available_inode();
    if(avail_inode == -1) {
        return -1;
    }
    int avail_block = find_available_block();
    if(avail_block == -1) {
        return -1;
    }
    inodes[avail_inode].first_block = avail_block;
    inodes[avail_inode].blocks_amount = 1;
    inodes[avail_inode].type = type;
    inodes[avail_inode].mydirent_amount = 0;
    blocks[avail_block].available = 0;
    return avail_inode;
}

int myopen(const char *pathname, int flags) {
    printf("my open called with  -> :%s:\n", pathname);
    char path[NAME_SIZE] = {'\0'};
    char* last_slash = strrchr(pathname, '/');
    strncpy(path, pathname, last_slash - pathname);
    myDIR* mdir = myopendir(path);
    mydirent* mrent = myreaddir(mdir);
    while(mrent != NULL) {
        if(strcmp(mrent->d_name, last_slash + 1) == 0) {
            if(flags == 1) {
                return -1;
            }
            fds[avail_fd].my_inode = mrent->inode_num;
            fds[avail_fd].currser = 0;
            return avail_fd++;
        }
        mrent = myreaddir(mdir);
    }
    int new_inode = allocate_file_folder(flags);
    attach_file_to_folder(mdir, new_inode, last_slash + 1);
    if(flags == 0) {
        fds[avail_fd].my_inode = new_inode;
        fds[avail_fd].currser = 0;
        myclosedir(mdir);
        return avail_fd++;
    }
    myclosedir(mdir);
    return -1;
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
    if(block_num >= inodes[fds[myfd].my_inode].blocks_amount) {
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
            inodes[fds[myfd].my_inode].blocks_amount++;
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
                inodes[fds[myfd].my_inode].blocks_amount++;
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
        fds[myfd].currser = inodes[fds[myfd].my_inode].blocks_amount * BLOCK_DATA_SIZE - offset;
        return fds[myfd].currser;
    }
    return -1;
}

void attach_file_to_folder(myDIR* mdir, int ino, const char* filename) {    
    mydirent md;
    strcpy(md.d_name, filename);
    md.inode_num = ino;

    char* ch_md = (char*)&md;
    int currser = inodes[mdir->inode].mydirent_amount * sizeof(mydirent);
    int block_num = currser / BLOCK_DATA_SIZE;
    int curr_block = inodes[mdir->inode].first_block;
    for(int i = 0; i < block_num; i++) {
        if(blocks[curr_block].next == -1) {
            int avail_block = find_available_block();
            if(avail_block == -1) {
                perror("find avail block");
                exit(0);
            }
            blocks[avail_block].available = 0;
            blocks[curr_block].next = avail_block;
            inodes[mdir->inode].blocks_amount++;
        }
        curr_block = blocks[curr_block].next;
    }
    for(int i = 0; i < sizeof(mydirent); i++) {
        if(currser != 0 && (currser % BLOCK_DATA_SIZE) == 0) {
            if(blocks[curr_block].next == -1) {
                int avail_block = find_available_block();
                if(avail_block == -1) {
                    perror("find avail block");
                    exit(0);
                }
                blocks[avail_block].available = 0;
                blocks[curr_block].next = avail_block;
                inodes[mdir->inode].blocks_amount++;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = currser % BLOCK_DATA_SIZE;
        blocks[curr_block].data[pos] = ch_md[i];
        currser++;
    }
    inodes[mdir->inode].mydirent_amount++;
    save_to_file();
}

myDIR* myopendir(const char *name) {
    // printf("myopendir called -> :%s:\n", name);
    if(strcmp(name, "\0") == 0) {
        myDIR* md = (myDIR*)malloc(sizeof(myDIR));
        if(md == NULL) {
            perror("no memory");
            return NULL;
        }
        md->inode = sb.root.inode_num;
        md->currser = 0;
        return md;
    }
    myDIR* mdir = myopendir("\0");
    if(mdir == NULL) {
        return NULL;
    }
    struct mydirent* curr_obj;
    char curr_path[NAME_SIZE];
    strcpy(curr_path, name);
    char f_name[NAME_SIZE];
    strcpy(f_name, name);
    char* path = strtok(f_name, "/");
    while(path != NULL) {
        curr_obj = myreaddir(mdir);
        if(curr_obj == NULL) {
            perror("invalid folder");
            return NULL;
        }
        // printf("comparing -> :%s: and :%s:\n", path, curr_obj->d_name);
        while(strcmp(path, curr_obj->d_name) != 0 && inodes[curr_obj->inode_num].type != 1) {
            free(curr_obj);
            curr_obj = myreaddir(mdir);
            if(curr_obj == NULL) {
                return NULL;
            }
            // printf("comparing -> :%s: and :%s:\n", path, curr_obj->d_name);
        }
        path = strtok(NULL, "/");
        if(path == NULL) {
            break;
        }
        strcat(curr_path, "/");
        strcat(curr_path, curr_obj->d_name);
        // printf("curr_path is-> :%s:\n", curr_path);
        mdir = myopendir(curr_path);
        if(mdir == NULL) {
            return NULL;
        }
        free(curr_obj);
    }
    free(curr_obj);
    return mdir;
}

struct mydirent* myreaddir(myDIR *dirp) {
    if(dirp->currser / sizeof(mydirent) >=  inodes[dirp->inode].mydirent_amount) {
        return NULL;
    }
    char* mrent = (char*)malloc(sizeof(mydirent));
    if(mrent == NULL) {
        perror("no memory");
        return NULL;
    }
    int block_num = dirp->currser / BLOCK_DATA_SIZE;
    if(block_num >= inodes[dirp->inode].blocks_amount) {
        return NULL;
    }
    int curr_block = inodes[dirp->inode].first_block;
    for(int i = 0; i < sizeof(mydirent); i++) {
        if(dirp->currser != 0 && dirp->currser % BLOCK_DATA_SIZE == 0) {
            if(blocks[curr_block].next == -1) {
                return NULL;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = dirp->currser % BLOCK_DATA_SIZE;
        mrent[i] = blocks[curr_block].data[pos];
        dirp->currser++;
    }
    return (struct mydirent*)mrent;
}

int myclosedir(myDIR *dirp) { // NOT DONE
    free(dirp);
    return 0;
}
