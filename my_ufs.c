#include "my_ufs.h"

superblock sb;
inode* inodes;
block* blocks;

myopenfile fds[MAX_FILES];
int avail_fd = 3;

void mymkfs(int s) {
    sb.inodes_amount = (s / sizeof(inode)) / 10;
    sb.blocks_amount = (s - (sb.inodes_amount * sizeof(inode)) - sizeof(superblock)) / sizeof(block);
    sb.block_size = sizeof(block);

    // printf("inodes: %d\n", sb.inodes_amount);
    // printf("blocks: %d\n", sb.blocks_amount);

    inodes = (inode*)malloc(sizeof(inode) * sb.inodes_amount);
    if(inodes == NULL) {
        perror("no memory");
    }
    for(int i = 0; i < sb.inodes_amount; i++) {
        inodes[i].first_block = NULL;
        strcpy(inodes[i].name, "-------");
        inodes[i].size = 0;
    }

    blocks = (block*)malloc(sizeof(block) * sb.blocks_amount);
    if(blocks == NULL) {
        perror("no memory");
    }
    for(int i = 0; i < sb.blocks_amount; i++) {
        blocks[i].next = NULL;
        blocks[i].available = 1;
    }

    mymount("myFile.txt");
}

int mymount(const char* dest) {
    FILE* fp;
    fp = fopen(dest, "w+");
    fwrite(&sb, sizeof(superblock), 1, fp);
    fwrite(inodes, sizeof(inode), sb.inodes_amount, fp);
    fwrite(blocks, sizeof(block), sb.blocks_amount, fp);    
    fclose(fp);
}

void myprint() {
    printf("superblock info\n");
    printf("\tinodes amount: %d\n", sb.inodes_amount);
    printf("\tblocks amount: %d\n", sb.blocks_amount);
    printf("\tblock size: %d\n", sb.block_size);

    printf("inodes:\n");
    for(int i = 0; i < sb.inodes_amount; i++) {
        printf("\tinode num: %d size: %d block: %p name: %s\n", i, inodes[i].size, inodes[i].first_block, inodes[i].name);
    }
    printf("blocks:\n");
    for(int i = 0; i < sb.blocks_amount; i++) {
        printf("\tblock num: %d next block: %p available: %d\n", i, blocks[i].next, blocks[i].available);
    }
}

int find_available_inode() {
    for(int i = 0; i < sb.inodes_amount; i++) {
        if(inodes[i].first_block == NULL) {
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

int allocate_file(const char *pathname) {
    int avail_inode = find_available_inode();
    if(avail_inode == -1) {
        return -1;
    }
    int avail_block = find_available_block();
    if(avail_block == -1) {
        return -1;
    }
    inodes[avail_block].first_block = &blocks[avail_block];
    inodes[avail_inode].size = 1;
    blocks[avail_block].available = 0;
    strcpy(inodes[avail_inode].name, pathname);
    return avail_inode;
}

int myopen(const char *pathname, int flags) {
    for(int i = 0; i < sb.inodes_amount; i++) {
        if(strcmp(pathname, inodes[i].name) == 0) {
            fds[avail_fd].my_inode = &inodes[i];
            fds[avail_fd].currser = 0;
            return avail_fd++;
        }
    }
    int new_inode = allocate_file(pathname);
    fds[avail_fd].my_inode = &inodes[new_inode];
    fds[avail_fd].currser = 0;
    return avail_fd++; 
}

int myclose(int myfd) {
    if(myfd >= avail_fd) {
        return -1;
    }
    fds[myfd].my_inode = NULL;
    return 0;
}

ssize_t myread(int myfd, void *buf, size_t count) {
    if(fds[myfd].my_inode == NULL) {
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].currser / BLOCK_DATA_SIZE;
    if(block_num >= fds[myfd].my_inode->size) {
        return -1;
    }
    block* curr_block = fds[myfd].my_inode->first_block;
    for(int i = 0; i < count; i++) {
        if(fds[myfd].currser != 0 && fds[myfd].currser % BLOCK_DATA_SIZE == 0) {
            if(curr_block->next == NULL) {
                return i;
            }
            curr_block = curr_block->next;
        }
        int pos = fds[myfd].currser % BLOCK_DATA_SIZE;
        text[i] = curr_block->data[pos];
        fds[myfd].currser++;
    }
    return count;
}

ssize_t mywrite(int myfd, const void *buf, size_t count) {
    if(fds[myfd].my_inode == NULL) {
        return -1;
    }
    char* text = (char*)buf;
    int block_num = fds[myfd].currser / BLOCK_DATA_SIZE;
    block* curr_block = fds[myfd].my_inode->first_block;
    for(int i = 0; i < block_num; i++) {
        if(curr_block->next == NULL) {
            int avail_block = find_available_block();
            if(avail_block == -1) {
                return -1;
            }
            blocks[avail_block].available = 0;
            curr_block->next = &blocks[avail_block];
            fds[myfd].my_inode->size++;
        }
        curr_block = curr_block->next;
    }
    for(int i = 0; i < count; i++) {
        if(fds[myfd].currser != 0 && (fds[myfd].currser % BLOCK_DATA_SIZE) == 0) {
            if(curr_block->next == NULL) {
                int avail_block = find_available_block();
                if(avail_block == -1) {
                    return -1;
                }
                blocks[avail_block].available = 0;
                curr_block->next = &blocks[avail_block];
                fds[myfd].my_inode->size++;
            }
            curr_block = curr_block->next;
        }
        int pos = fds[myfd].currser % BLOCK_DATA_SIZE;
        curr_block->data[pos] = text[i];
        fds[myfd].currser++;
    }
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
        fds[myfd].currser = fds[myfd].my_inode->size * BLOCK_DATA_SIZE - offset;
        return fds[myfd].currser;
    }
    return -1;
}