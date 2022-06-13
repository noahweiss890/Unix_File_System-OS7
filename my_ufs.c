#include "my_ufs.h"

superblock sb;
inode* inodes;
block* blocks;
myopenfile fds[MAX_FILES];
int avail_fd = 3;

void mymkfs(int s) {
    if(s <= 0) {
        perror("s must be positive");
        return;
    }
    sb.inodes_amount = (s / sizeof(inode)) / 10;
    sb.blocks_amount = (s - (sb.inodes_amount * sizeof(inode)) - sizeof(superblock)) / sizeof(block);
    sb.block_size = sizeof(block);

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
        inodes[i].length = 0;
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
    save_to_file("myFile.txt");
}

void free_mem() {
    if(inodes != NULL && blocks != NULL) {
        free(inodes);
        free(blocks);
        printf("freed\n");
    }
    else {
        perror("error");
    }
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
    if(inodes != NULL && blocks != NULL) {
        free_mem();
    }
    if (source == NULL && target == NULL) {
        perror("error");
        return -1;
    }
    if (source != NULL) {
        FILE* fp;
        fp = fopen(source, "r");
        if(fp == NULL) {
            perror("fopen");
            return -1;
        }
        fread(&sb, sizeof(superblock), 1, fp);
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
        fread(inodes, sizeof(inode), sb.inodes_amount, fp);
        fread(blocks, sizeof(block), sb.blocks_amount, fp);    
    
        fclose(fp);
    }
    if(target != NULL) {
        save_to_file(target);
    }
    return 0;
}

void save_to_file(const char *pathname) {
    if(inodes == NULL || blocks == NULL) {
        perror("error");
        return;
    }
    FILE* fp;
    fp = fopen(pathname, "w");
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
    if(inodes == NULL || blocks == NULL) {
        perror("error");
        return;
    }
    printf("superblock info:\n");
    printf("\tinodes amount: %d\n", sb.inodes_amount);
    printf("\tblocks amount: %d\n", sb.blocks_amount);
    printf("\tblock size: %d\n", sb.block_size);

    printf("inodes:\n");
    for(int i = 0; i < sb.inodes_amount; i++) {
        printf("\tinode num: %d size: %d first block: %d amount: %d type: %d length: %d\n", i, inodes[i].blocks_amount, inodes[i].first_block, inodes[i].mydirent_amount, inodes[i].type, inodes[i].length);
    }
    printf("blocks:\n");
    for(int i = 0; i < sb.blocks_amount; i++) {
        printf("\tblock num: %d next block: %d available: %d\n", i, blocks[i].next, blocks[i].available);
    }
}

int find_available_inode() {
    if(inodes == NULL) {
        perror("error");
        return -1;
    }
    for(int i = 0; i < sb.inodes_amount; i++) {
        if(inodes[i].first_block == -1) {
            return i;
        }
    }
    return -1;
}

int find_available_block() {
    if(blocks == NULL) {
        perror("error");
        return -1;
    }
    for(int i = 0; i < sb.blocks_amount; i++) {
        if(blocks[i].available == 1) {
            return i;
        }
    }
    return -1;
}

int allocate_file_folder(int type) {
    if(inodes == NULL || blocks == NULL) {
        perror("error");
        return -1;
    }
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
    inodes[avail_block].length = 0;
    blocks[avail_block].available = 0;
    return avail_inode;
}

int myopen(const char *pathname, int flags) {
    if(inodes == NULL || blocks == NULL || pathname == NULL) {
        perror("error");
        return -1;
    }
    char path[NAME_SIZE] = {'\0'};
    char* last_slash = strrchr(pathname, '/');
    strncpy(path, pathname, last_slash - pathname);
    myDIR* mdir = myopendir(path);
    if(mdir == 0) {
        return -1;
    }
    mydirent* mrent = myreaddir(mdir);
    while(mrent != NULL) {
        if(strcmp(mrent->d_name, last_slash + 1) == 0) {
            fds[avail_fd].my_inode = mrent->inode_num;
            if(flags & O_APPEND) {
                fds[avail_fd].curser = inodes[mrent->inode_num].length;
            }
            else {
                fds[avail_fd].curser = 0;
            }
            return avail_fd++;
        }
        free(mrent);
        mrent = myreaddir(mdir);
    }
    if(flags & O_CREAT) {
        int new_inode = allocate_file_folder(0);
        attach_to_folder(mdir, new_inode, last_slash + 1);
        fds[avail_fd].my_inode = new_inode;
        fds[avail_fd].curser = 0;
        myclosedir(mdir);
        return avail_fd++;
    }
    free(mrent);
    myclosedir(mdir);
    return -1;
}

int myclose(int myfd) {
    if(inodes == NULL || blocks == NULL) {
        perror("error");
        return -1;
    }
    if(myfd >= avail_fd || fds[myfd].my_inode == -1) {
        return -1;
    }
    fds[myfd].my_inode = -1;
    return 0;
}

ssize_t myread(int myfd, void *buf, size_t count) {
    if(inodes == NULL || blocks == NULL || fds[myfd].my_inode == -1) {
        perror("error");
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].curser / BLOCK_DATA_SIZE;
    if(block_num >= inodes[fds[myfd].my_inode].blocks_amount) {
        return -1;
    }
    int curr_block = inodes[fds[myfd].my_inode].first_block;
    for(int i = 0; i < block_num; i++) {
        curr_block = blocks[curr_block].next;
    }
    for(int i = 0; i < count; i++) {
        if(fds[myfd].curser != 0 && fds[myfd].curser % BLOCK_DATA_SIZE == 0) {
            if(blocks[curr_block].next == -1) {
                return i;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = fds[myfd].curser % BLOCK_DATA_SIZE;
        text[i] = blocks[curr_block].data[pos];
        fds[myfd].curser++;
    }
    return count;
}

ssize_t mywrite(int myfd, const void *buf, size_t count) {
    if(inodes == NULL || blocks == NULL || fds[myfd].my_inode == -1) {
        perror("error");
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].curser / BLOCK_DATA_SIZE;
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
        if(fds[myfd].curser != 0 && (fds[myfd].curser % BLOCK_DATA_SIZE) == 0) {
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
        int pos = fds[myfd].curser % BLOCK_DATA_SIZE;
        blocks[curr_block].data[pos] = text[i];
        fds[myfd].curser++;
        inodes[fds[myfd].my_inode].length++;
    }
    save_to_file("myFile.txt");
    return count;
}

off_t mylseek(int myfd, off_t offset, int whence) {
    if(inodes == NULL || blocks == NULL) {
        perror("error");
        return -1;
    }
    if(whence == SEEK_SET) {
        fds[myfd].curser = offset;
        return fds[myfd].curser;
    }
    else if(whence == SEEK_CUR) {
        fds[myfd].curser += offset;
        return fds[myfd].curser;
    }
    else if(whence == SEEK_END) {
        fds[myfd].curser = inodes[fds[myfd].my_inode].length - offset;
        return fds[myfd].curser;
    }
    return -1;
}

void attach_to_folder(myDIR* mdir, int ino, const char* filename) {
    if(inodes == NULL || blocks == NULL || mdir == NULL || filename == NULL) {
        perror("error");
        return;
    }
    mydirent md;
    strcpy(md.d_name, filename);
    md.inode_num = ino;

    char* ch_md = (char*)&md;
    int curser = inodes[mdir->inode].mydirent_amount * sizeof(mydirent);
    int block_num = curser / BLOCK_DATA_SIZE;
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
        if(curser != 0 && (curser % BLOCK_DATA_SIZE) == 0) {
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
        int pos = curser % BLOCK_DATA_SIZE;
        blocks[curr_block].data[pos] = ch_md[i];
        curser++;
    }
    inodes[mdir->inode].mydirent_amount++;
    save_to_file("myFile.txt");
}

myDIR* myopendir(const char *name) {
    if(inodes == NULL || blocks == NULL || name == NULL) {
        perror("error");
        return NULL;
    }
    if(strcmp(name, "\0") == 0) {
        myDIR* md = (myDIR*)malloc(sizeof(myDIR));
        if(md == NULL) {
            perror("no memory");
            return NULL;
        }
        md->inode = sb.root.inode_num;
        md->curser = 0;
        return md;
    }
    char parent_folder[NAME_SIZE] = {'\0'};
    strncpy(parent_folder, name, strrchr(name, '/') - name);
    myDIR* mdir = myopendir(parent_folder);
    if(mdir == NULL) {
        return NULL;
    }
    struct mydirent* curr_obj;
    curr_obj = myreaddir(mdir);
    while(curr_obj != NULL) {
        if(inodes[curr_obj->inode_num].type == 1 && strcmp(curr_obj->d_name, strrchr(name, '/') + 1) == 0) {
            mdir->inode = curr_obj->inode_num;
            mdir->curser = 0;
            free(curr_obj);
            return mdir;
        }
        free(curr_obj);
        curr_obj = myreaddir(mdir);
    }
    free(curr_obj);
    int ino = allocate_file_folder(1);
    attach_to_folder(mdir, ino, strrchr(name, '/') + 1);
    mdir->inode = ino;
    mdir->curser = 0;
    return mdir;
}

struct mydirent* myreaddir(myDIR *dirp) {
    if(inodes == NULL || blocks == NULL || dirp == NULL) {
        perror("error");
        return NULL;
    }
    if(dirp->curser / sizeof(mydirent) >=  inodes[dirp->inode].mydirent_amount) {
        return NULL;
    }
    char* mrent = (char*)malloc(sizeof(mydirent));
    if(mrent == NULL) {
        perror("no memory");
        return NULL;
    }
    int block_num = dirp->curser / BLOCK_DATA_SIZE;
    if(block_num >= inodes[dirp->inode].blocks_amount) {
        return NULL;
    }
    int curr_block = inodes[dirp->inode].first_block;
    for(int i = 0; i < sizeof(mydirent); i++) {
        if(dirp->curser != 0 && dirp->curser % BLOCK_DATA_SIZE == 0) {
            if(blocks[curr_block].next == -1) {
                return NULL;
            }
            curr_block = blocks[curr_block].next;
        }
        int pos = dirp->curser % BLOCK_DATA_SIZE;
        mrent[i] = blocks[curr_block].data[pos];
        dirp->curser++;
    }
    return (struct mydirent*)mrent;
}

int myclosedir(myDIR *dirp) {
    if(dirp == NULL) {
        perror("error");
        return -1;
    }
    free(dirp);
    return 0;
}
