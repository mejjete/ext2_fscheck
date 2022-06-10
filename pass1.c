#include <ext2fs.h>
#include <tree.h>

/* Inode and data bitmaps */
static u8 *inode_bitmap;
static u8 *data_bitmap;

void ext2_fsck_pass1()
{
    /**
     * Inode and data bitmaps always occupy 1 block
     */
    if((inode_bitmap = malloc(block_size)) == NULL)
        err_sys("memory exhausted");
    
    if((data_bitmap = malloc(block_size)) == NULL)
    {
        free(inode_bitmap);
        err_sys("memory exhausted");
    }

    
}