#include <ext2fs.h>
#include <math.h>
#include <misc.h>


typedef struct ext2_block_stream
{
    dev_t device;                   /* Device ID associated with file system device */
    u32 index;                      /* Index in directory structure */
    u32 d_blocks[EXT2_IND_BLOCK];   /* Direct blocks */
    u32 i_block;                    /* Indirect block */
    u32 di_block;                   /* Doubly-indirect block */
    u32 ti_block;                   /* Thriply-indirect block */
} ext2_BLK;


typedef struct ext2_directory_stream
{
    u32 offset;                 /* Offset within directory block */
    block_t block;              /* Directory block */
    ext2_BLK block_stream;      /* Open stream of an associated directory blocks */
    struct ext2_inode inode;    /* Inode instance */
} ext2_DIR;


ext2_BLK *ext2_open_blk(ext2_context_t *fs_ctx, struct ext2_inode *inode)
{
    ext2_BLK *block_stream;
    if((block_stream = malloc(sizeof(ext2_BLK))) == NULL)
        return NULL;
    
    block_stream->device = fs_ctx->sb_wrap.device;
    block_stream->index = 0;

    memcpy(block_stream->d_blocks, inode->i_block, sizeof(u32) * EXT2_IND_BLOCK);
    block_stream->i_block = inode->i_block[12];
    block_stream->di_block = inode->i_block[13];
    block_stream->ti_block = inode->i_block[14];
    return block_stream;
}


static block_t read_ind_block(dev_t dev, block_t block, u32 index)
{
    block_seek(dev, block, SEEK_SET);
    lseek64(dev, index * sizeof(u32), SEEK_CUR);
    block_t blk;
    read(dev, &blk, sizeof(block_t));
    return blk;
}


static block_t get_next_blk(ext2_BLK *blk_stream)
{
    u32 blks_per_ind = block_size / 4;
    u32 blks_per_dind = blks_per_ind * blks_per_ind; 
    u32 blks_per_tind = blks_per_ind * blks_per_ind * blks_per_ind;
    block_t ret_block = 0;
    u32 index = blk_stream->index;

    if(index >= EXT2_IND_BLOCK + blks_per_ind + blks_per_dind + blks_per_tind)
    {
        u32 local_index = index - (EXT2_IND_BLOCK + blks_per_ind + blks_per_dind + blks_per_tind);
        u32 di_blk_ind = local_index / blks_per_dind;
        u32 i_blk_ind = local_index / blks_per_ind;
        u32 blk_ind = local_index % blks_per_ind;
        
        block_t di_block = read_ind_block(blk_stream->device, blk_stream->di_block, di_blk_ind);
        block_t i_block = read_ind_block(blk_stream->device, di_block, i_blk_ind);
        ret_block = read_ind_block(blk_stream->device, i_block, blk_ind);
    }
    else if(index >= EXT2_IND_BLOCK + blks_per_ind + blks_per_dind)
    {
        u32 local_index = index - (EXT2_IND_BLOCK + blks_per_ind);
        u32 main_blk_ind = local_index / blks_per_ind;
        u32 rel_blk_ind = local_index % blks_per_ind;
        
        block_t main_blk = read_ind_block(blk_stream->device, blk_stream->di_block, main_blk_ind);
        ret_block = read_ind_block(blk_stream->device, main_blk, rel_blk_ind);
    }
    else if(index >= EXT2_IND_BLOCK + blks_per_ind)
    {
        u32 i_blk_ind = index - EXT2_IND_BLOCK;
        ret_block = read_ind_block(blk_stream->device, blk_stream->i_block, i_blk_ind); 
    }
    else
        ret_block = blk_stream->d_blocks[index];

    blk_stream->index++;
    return ret_block;
}


block_t ext2_read_blk(ext2_BLK *blk_stream)
{
    block_t block;
    
    do
    {
        block = get_next_blk(blk_stream);
    } while(block == 0);

    return block;
}


ext2_DIR *ext2_open_dir(ext2_context_t *fs_ctx, ino_t inode)
{
    struct ext2_inode ind = *ext2_get_inode_entry(&fs_ctx->sb_wrap, inode);
    if(!S_ISDIR(ind.i_mode))
        return NULL;

    if((ind.i_flags & EXT2_INDEX_FL) == EXT2_INDEX_FL)
        return NULL;        /* Does not support indexed directory structure */

    ext2_DIR *dir_stream;
    if((dir_stream = malloc(sizeof(ext2_DIR))) == NULL)
        err_sys("memory exhausted");

    dir_stream->offset = 0;
    dir_stream->inode = ind;
    dir_stream->block_stream = *ext2_open_blk(fs_ctx, &ind);
    dir_stream->block = ext2_read_blk(&dir_stream->block_stream);

    return dir_stream;
}


struct ext2_dir_entry_2 *ext2_read_dir(ext2_context_t *fs_ctx, ext2_DIR *direntry)
{
    struct ext2_sb_wrap *sb_wrap = &fs_ctx->sb_wrap;
    dev_t device = sb_wrap->device;
    
    block_seek(device, direntry->block, SEEK_SET);
    lseek64(device, direntry->offset, SEEK_CUR);
    u8 buff[16];
    read(device, buff, 16);

    u16 rec_len = *((u16 *) (buff + 4));
    struct ext2_dir_entry_2 *dir;
    if((dir = malloc(rec_len)) == NULL)
        err_sys("memory exhausted");
    
    dir->inode = *((u16 *) buff);
    dir->rec_len = rec_len;
    dir->name_len = *((u8 *) (buff + 6));
    dir->file_type = *((u8 *) (buff + 7));
    read(device, dir->name, dir->name_len);
    
    direntry->offset += dir->rec_len;
    return dir;
}


void ext2_close_stream(void *mem)
{
    if(mem)
        free(mem);
}