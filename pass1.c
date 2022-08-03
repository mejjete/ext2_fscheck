#include <ext2fs.h>
#include <misc.h>
#include <directory.h>
#include <util.h>
#include <ino_ops.h>


static bool check_blk(u32 upper, u32 lower, block_t blk)
{
    if(blk > upper || blk < lower)
        return false;
    return true;
}

block_t get_block(ext2_DIR *);

void ext2_fsck_pass1(ext2_context_t *fs_ctx, ino_t dir_ino)
{
    ext2_DIR *dir = ext2_open_dir(fs_ctx, dir_ino);
    if(dir == NULL)
    {
        printf("Inode %ld is not a directory\n", dir_ino);
        return;
    }

    struct ext2_dir_entry_2 *direntry;
    while((direntry = ext2_read_dir(fs_ctx, dir)) != NULL)
    {
        if(strcmp(direntry->name, ".") == 0 || 
            strcmp(direntry->name, "..") == 0)
            continue;
   

        /* Check inode as unconnected object */
        struct ext2_inode *inode = ext2_get_inode(fs_ctx, direntry->inode);     
        ext2_err_t errcode = ext2_check_inode(inode);
        if(errcode != EXT2_NO_ERR)
            printf("Inode %d has invalid inode format\n", direntry->inode);

        /* Check if a blocks occupied by an inode are within a valid range */
        ext2_BLK *blk_stream = ext2_open_blk(fs_ctx, inode);
        struct block_struct *blk;
        u32 blk_lower = fs_ctx->sb.s_first_data_block;
        u32 blk_upper = fs_ctx->sb.s_blocks_count; 

        while((blk = ext2_read_blk(blk_stream)) != NULL)
        {
            if(!check_blk(blk_upper, blk_lower, blk->block))
            {
                printf("Inode %d contains invalid block number: %d\n", 
                    direntry->inode, blk->block);
                break;
            }
            ext2_set_bm(fs_ctx, fs_ctx->data_bitmap, blk->block);
        }
        ext2_free_blk(blk_stream);

        if(EXT2_FT_ISDIR(direntry->file_type))
        {
            ext2_set_bm(fs_ctx, fs_ctx->dir_bitmap, direntry->inode);
            if(strcmp(direntry->name, "lost+found") == 0)
                continue;
            ext2_fsck_pass1(fs_ctx, direntry->inode);
        }
        ext2_set_bm(fs_ctx, fs_ctx->inode_bitmap, direntry->inode);
    }

    ext2_close_dir(dir);
}