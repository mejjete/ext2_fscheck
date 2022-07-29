#include <ino_ops.h>


typedef struct ext2_block_stream
{
    dev_t device;                   /* Device ID associated with file system device */
    u32 index;                      /* Index in directory structure */
    u32 d_blocks[EXT2_IND_BLOCK];   /* Direct blocks */
    u32 i_block;                    /* Indirect block */
    u32 di_block;                   /* Double-indirect block */
    u32 ti_block;                   /* Triple-indirect block */
    struct indirect_blk *iblk;      /* Indirect block stream */
} ext2_BLK;


struct indirect_blk
{
    block_t block;
    u32 index;
    u32 dim;
    struct indirect_blk *next;
};


static block_t read_block(dev_t dev, block_t block, u32 index)
{
    block_seek(dev, block, SEEK_SET);
    lseek64(dev, index * sizeof(u32), SEEK_CUR);
    block_t blk;
    read(dev, &blk, sizeof(block_t));
    return blk;
}


static struct block_struct make_blk_context(block_t block, block_type blk_type)
{
    struct block_struct retblock;
    retblock.block = block;
    retblock.type = blk_type;
    return retblock;
}


static void free_ind_blk(struct indirect_blk *blk)
{
    while (blk != NULL)
    {
        blk = blk->next;
        free(blk);
    }
}


ext2_BLK *ext2_open_blk(ext2_context_t *fs_ctx, struct ext2_inode *inode)
{
    ext2_BLK *blk_stream;

    if((blk_stream = malloc(sizeof(ext2_BLK))) == NULL)
        err_sys("memory exhausted");
    
    blk_stream->device = fs_ctx->device;
    blk_stream->index = 0;

    memcpy(blk_stream->d_blocks, inode->i_block, sizeof(u32) * EXT2_IND_BLOCK);
    blk_stream->i_block = inode->i_block[EXT2_IND_BLOCK];
    blk_stream->di_block = inode->i_block[EXT2_DIND_BLOCK];
    blk_stream->ti_block = inode->i_block[EXT2_TIND_BLOCK];
    blk_stream->iblk = NULL;
    return blk_stream;
}


static struct indirect_blk *open_ind_blk(dev_t device, block_t block, u32 dimension)
{
    if(dimension == 0 || dimension > 3)
        return NULL;

    struct indirect_blk *iblock = malloc(sizeof(struct indirect_blk));
    struct indirect_blk *iter = iblock;
    iblock->block = block;
    iblock->dim = dimension;
    iblock->index = 0;
    dimension--;

    for(u32 i = dimension; i > 0; i--)
    {
        block_t block = read_block(device, iter->block, 0);
        if((iter->next = malloc(sizeof(struct indirect_blk))) == NULL)
            err_sys("memory exhausted");

        iter->next->dim = i;
        iter->next->block = block;
        iter->next->index = 0;
        iter = iter->next;
    }

    return iblock;
}


static struct block_struct *read_ind_blk(dev_t device, struct indirect_blk *iblk)
{   
    u32 entry_per_block = (block_size / 4);

    /**
     * For indirect block indexing starts with 1 since 
     * 0 value represents a parent block id
     */
    if(iblk->index > entry_per_block)
        return NULL;
    
    struct block_struct *retblock;
    static struct block_struct temp;

    if(iblk->index == 0)
    {
        temp = make_blk_context(iblk->block, DIRECT_BLK);
        iblk->index++;
        return &temp;
    }

    block_t blk = read_block(device, iblk->block, iblk->index - 1);
    temp = make_blk_context(blk, INDIRECT_BLK);

    /* Stream tail points to a direct block */
    if(iblk->next == NULL)
    {
        iblk->index++;
        return &temp;
    }
    else
    {
        retblock = read_ind_blk(device, iblk->next);

        if(retblock != NULL)
            return retblock;
        else 
        {
            u32 dim = iblk->next->dim;
            free_ind_blk(iblk->next);

            if(iblk->index + 1 > entry_per_block)
                return NULL;

            iblk->index++;
            iblk->next = open_ind_blk(device, blk, dim);
            return read_ind_blk(device, iblk->next);
        }
    }
}


struct block_struct *ext2_read_blk(ext2_BLK *blk_stream)
{
    u32 blks_per_ind = block_size / 4;
    u32 blks_per_dind = blks_per_ind * blks_per_ind; 
    u32 index = blk_stream->index;
    dev_t device = blk_stream->device;
    block_t cblock;
    u32 dim;
    static struct block_struct blk;
    struct block_struct *b;

    // Direct blocks
    if(index < EXT2_IND_BLOCK)
    {
        blk = make_blk_context(blk_stream->d_blocks[index], DIRECT_BLK);
        blk_stream->index++;
        if(blk.block == 0)
            return NULL;
        return &blk;
    }

    while(1)
    {
        if(blk_stream->iblk != NULL)
        {
            b = read_ind_blk(device, blk_stream->iblk);
            if(b != NULL)
                break;
            else 
                blk_stream->iblk = NULL;
        }

        // Indirect blocks
        if(index >= EXT2_IND_BLOCK + blks_per_ind + blks_per_dind)
        {
            cblock = blk_stream->ti_block;
            dim = 3;
        }
        else if(index >= EXT2_IND_BLOCK + blks_per_ind)
        {
            cblock = blk_stream->di_block;
            dim = 2;
        }
        else if(index >= EXT2_IND_BLOCK)
        {
            cblock = blk_stream->i_block;
            dim = 1;
        }

        blk_stream->iblk = open_ind_blk(device, cblock, dim);  
    }

    if(b->block == 0)
        return NULL;

    blk_stream->index++;
    return b;
}


block_t ext2_read_data_blk(ext2_BLK *blk_stream)
{
    struct block_struct *blk;

    while((blk = ext2_read_blk(blk_stream)) != NULL)
    {
        if(blk->type == DIRECT_BLK)
            return blk->block;
    }
    return 0;
}


void ext2_free_blk(ext2_BLK *blk_stream)
{   
    if(blk_stream == NULL)
        return;

    free_ind_blk(blk_stream->iblk);
    free(blk_stream);
}