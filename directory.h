#ifndef EXR2_DIRECTORY_H
#define EXR2_DIRECTORY_H

#include <ext2fs.h>

typedef struct ext2_directory_stream ext2_DIR;
typedef struct ext2_block_stream ext2_BLK;

/* directory.c */

/**/
ext2_DIR *ext2_open_dir(ext2_context_t *, block_t);

/**/
ext2_BLK *ext2_open_blk(ext2_context_t *, struct ext2_inode *);

/**/
block_t ext2_read_blk(ext2_BLK *);

/**/
struct ext2_dir_entry_2 *ext2_read_dir(ext2_context_t *, ext2_DIR *);

/**/
void ext2_close_stream(void *);

#endif // EXR2_DIRECTORY_H