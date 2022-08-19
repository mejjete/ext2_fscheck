#include <ext2fs.h>
#include <directory.h>
#include <math.h>
#include <misc.h>
#include <ino_ops.h>


typedef struct ext2_directory_stream
{
    u32 offset;                 /* Offset within directory block */
    block_t block;              /* Directory block */
    ext2_BLK *block_stream;     /* Block stream */
    struct ext2_inode inode;    /* Inode instance */
} ext2_DIR;


ext2_DIR *ext2_open_dir(ext2_context_t *fs_ctx, ino_t inode)
{
    struct ext2_inode ind = *ext2_get_inode(fs_ctx, inode);
    if(!EXT2_S_ISDIR(ind.i_mode))
        return NULL;

    ext2_DIR *dir_stream;
    if((dir_stream = malloc(sizeof(ext2_DIR))) == NULL)
        err_sys("memory exhausted");

    dir_stream->offset = 0;
    dir_stream->inode = ind;
    dir_stream->block_stream = ext2_open_blk(fs_ctx, &ind);
    dir_stream->block = ext2_read_data_blk(dir_stream->block_stream);
    return dir_stream;
}


struct ext2_dir_entry_2 *ext2_read_dir(ext2_context_t *fs_ctx, ext2_DIR *direntry)
{
    if(!fs_ctx && !direntry)
        return NULL;

    /* Directory structure is damaged */
    if(direntry->offset > block_size)
        return NULL;

    /**
     * If offset within directory is equal to block size, it means
     * that next directory entry is located right in the next block
     */    
    if(direntry->offset == block_size)
    {
        direntry->offset = 0;
        block_t next_block = ext2_read_data_blk(direntry->block_stream);
        if(next_block == 0)
            return NULL;
        direntry->block = next_block;
    }

    dev_t device = fs_ctx->device;
    
    block_seek(device, direntry->block, SEEK_SET);
    lseek64(device, direntry->offset, SEEK_CUR);
    u8 buff[8];
    read(device, buff, 8);

    u16 rec_len = *((u16 *) (buff + 4));
    struct ext2_dir_entry_2 *dir;
    if((dir = malloc(rec_len)) == NULL)
        err_sys("memory exhausted");

    dir->inode = *((u32 *) buff);
    dir->rec_len = rec_len;
    dir->name_len = *((u8 *) (buff + 6));
    dir->file_type = *((u8 *) (buff + 7));
    read(device, dir->name, dir->name_len);
    direntry->offset += dir->rec_len;

    if(dir->inode == 0)
    {
        free(dir);
        return NULL;
    }
    return dir;
}


void ext2_close_dir(ext2_DIR *dr)
{
    if(dr)
        if(dr->block_stream)
            free(dr->block_stream);
    free(dr);
}