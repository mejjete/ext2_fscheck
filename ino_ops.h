#ifndef EXT2_INODE_OP_H
#define EXT2_INODE_OP_H


#include <ext2fs.h>
#include <ext2_config.h>


typedef struct ext2_block_stream        ext2_BLK;


enum block_identity
{
    DIRECT_BLK,             /* Real block */
    INDIRECT_BLK            /* Indirect block */
} typedef block_type;


struct block_struct 
{
    block_t     block;      /* Block number */
    block_type  type;       /* Block type */
};


/* block.c */


/**
 * @brief Opens inode block array as stream.
 * @return Pointer to heap-allocated handler, NULL if error is occurred.
 */
ext2_BLK *ext2_open_blk(ext2_context_t *, struct ext2_inode *);


/**
 * @brief Reads one-per-call block entry from block stream.
 * @return Next block number.
 */
struct block_struct *ext2_read_blk(ext2_BLK *);


/**
 * @brief 
 * @return block_t 
 */
block_t ext2_read_data_blk(ext2_BLK *);


/**/
void ext2_free_blk(ext2_BLK *);


#endif // EXT2_INODE_OP_H