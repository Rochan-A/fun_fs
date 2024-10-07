#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global vars.

// Superblock.
// Inode within this block we want to work with interally.

struct superblock sb;

// Array of what is currently open.
struct inode *inodes;
struct disk_block *disk_blocks;


void create_fs() {
    sb.num_inodes = 10;
    sb.num_blocks = 100;
    sb.size_blocks = sizeof(struct disk_block);

    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    for (int i = 0; i < sb.num_inodes; ++i){
        inodes[i].size = -1; // if it is not allocated.
        inodes[i].first_block = -1; // not valid to start.
        strcpy(inodes[i].name, "emptyfi");
    }

    disk_blocks = malloc(sizeof(struct disk_block) * sb.num_blocks);
    for (int i = 0; i < sb.num_blocks; ++i){
        disk_blocks[i].next_block_num = -1; // if it is not allocated.
    }
}

void sync_fs() {
    FILE *file;

    file = fopen("fs_data", "w+");

    fwrite(&sb, sizeof (struct superblock), 1, file);

    for (int i = 0; i < sb.num_inodes; ++i){
        fwrite(&inodes[i], sizeof(struct inode), 1, file);
    }

    for (int i = 0; i < sb.num_blocks; ++i){
        fwrite(&disk_blocks[i], sizeof(struct disk_block), 1, file);
    }

    fclose(file);
}

void mount_fs() {
    FILE *file;
    file = fopen("fs_data", "r");

    fread(&sb, sizeof (struct superblock), 1, file);

    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    disk_blocks = malloc(sizeof(struct disk_block) * sb.num_blocks);

    fread(inodes, sizeof(struct inode), sb.num_inodes, file);
    fread(disk_blocks, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);
}

void print_fs() {
    printf("Super block info\n");
    printf("\tnum_inodes=%d\n", sb.num_inodes);
    printf("\tnum_blocks=%d\n", sb.num_blocks);
    printf("\tsize_blocks=%d\n", sb.size_blocks);

    printf("inodes\n");
    for (int i = 0; i < sb.num_inodes; ++i){
        printf("\tsize: %d name: %s block: %d\n", inodes[i].size, inodes[i].name, inodes[i].first_block);
    }

    printf("disk blocks\n");
    for (int i = 0; i < sb.num_blocks; ++i){
        printf("\tcurrent_disk_block: %d next_disk_block: %d\n", i, disk_blocks[i].next_block_num);
    }
}

int find_empty_inode() {
    for(int i = 0; i < sb.num_inodes; ++i) {
        if (inodes[i].first_block == -1) {
            return i;
        }
    }
    return -1;
}

int find_empty_block() {
    // -1 == empty
    // -2 == used

    for(int i = 0; i < sb.num_blocks; ++i) {
        if (disk_blocks[i].next_block_num == -1) {
            return i;
        }
    }
    return -1;
}

int allocate_file(char name[8]) {
    // find an empty inode
    int fd = find_empty_inode();
    // find/claim a disk block
    int block = find_empty_block();
    // claim it
    inodes[fd].first_block = block;

    disk_blocks[block].next_block_num = -2;
    // return the fd

    strcpy(inodes[fd].name, name);

    return fd;
}

void shorten_file(int bn) {
    int nn = disk_blocks[bn].next_block_num;
    if (nn >= 0) {
        shorten_file(nn);
    }
    disk_blocks[bn].next_block_num = -1;
}

void set_filesize(int fd, int size){
    int tmp = size + BLOCKSIZE - 1;
    int num = tmp / BLOCKSIZE;

    int bn = inodes[fd].first_block;
    num--;

    while (num > 0) {
        int next_num = disk_blocks[bn].next_block_num;
        if (next_num == -2) {
            int empty = find_empty_block();
            disk_blocks[bn].next_block_num = empty;
            disk_blocks[empty].next_block_num = -2;
        }
        bn = disk_blocks[bn].next_block_num;
        num--;
    }

    // shorten if neccessary.
    shorten_file(bn);
    disk_blocks[bn].next_block_num = -2;
}


int get_block_num(int fd, int offset) {
    int togo = offset;
    int bn = inodes[fd].first_block;
    while (togo > 0) {
        bn = disk_blocks[bn].next_block_num;
        togo--;
    }
    return bn;
}

// Write data to file.
void write_byte(int fd, int pos, char *data) {
    // calc which block.
    int relative_block = pos / BLOCKSIZE;
    // find block num.
    int bn = get_block_num(fd, relative_block);
    // calc offset in block.
    int offset = pos % BLOCKSIZE;
    // write the data.
    disk_blocks[bn].data[offset] = (*data);
}