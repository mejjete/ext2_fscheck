#ifndef EXR2_DIRECTORY_H
#define EXR2_DIRECTORY_H
 

#include <ext2fs.h>


#define EXT2_FT_UNKNOWN         0
#define EXT2_FT_REG_FILE        1
#define EXT2_FT_DIR             2
#define EXT2_FT_CHRDEV          3
#define EXT2_FT_BLKDEV          4
#define EXT2_FT_FIFO            5
#define EXT2_FT_SOCK            6
#define EXT2_FT_SYMLINK         7


#define EXT2_FT_ISDIR(MODE)    (((MODE) & EXT2_FT_DIR) == EXT2_FT_DIR)  


typedef struct ext2_directory_stream    ext2_DIR;


/* directory.c */


/**
 * @brief Opens directory with specified inode number.
 * @return Block stream object.
 */
ext2_DIR *ext2_open_dir(ext2_context_t *, ino_t);


/**
 * @brief Reads one-per-call entry from directory stream.
 * @return Pointer to statically-allocated directory entry handler, 
 * NULL if error is occurred or end of directory stream is reached. 
 */
struct ext2_dir_entry_2 *ext2_read_dir(ext2_context_t *, ext2_DIR *);


/** @brief Frees directory stream */
static inline void ext2_close_dir(ext2_DIR *dr)
{
    if(dr)
        free(dr);
}


#endif // EXR2_DIRECTORY_H