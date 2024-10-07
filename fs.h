
// Metadata information about the filesystem.
// number of inodes
// number of disk blocks
// size of the disk blocks
// (could do size of inodes but this is simplified version)

#define BLOCKSIZE 512

struct superblock {
    int num_inodes;
    int num_blocks; // disk blocks
    int size_blocks;
};

struct inode {
    int size;
    int first_block;
    char name[8];
};

struct disk_block {
    int next_block_num;
    char data[BLOCKSIZE];
};

// Initialize new filesystem.
void create_fs();

// Load a filesystem.
void mount_fs();

// write the filesystem.
void sync_fs();

// Print filesystem data structure.
void print_fs();

// Returns fd.
int allocate_file(char name[8]);

// Set the size of the file. (grow or shrink file).
void set_filesize(int fd, int size);

// Write data to file.
void write_byte(int fd, int pos, char *data);

// delete
// read byte