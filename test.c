#include <stdio.h>
#include "fs.h"

int main() {
    // create_fs();

    // sync_fs();
    mount_fs();

    // allocate_file("first");
    // sync_fs();
    set_filesize(0, 5000);
    char data = 'h';
    for(int i = 0; i < 49; ++i) {
        write_byte(0, i * 100, &data);
    }
    sync_fs();

    print_fs();

    return 0;
}