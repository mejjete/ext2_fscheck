#include <ext2fs.h>
#include <util.h>
#include <misc.h>


extern size_t block_size;


int ext2_is_grp_contains_sb(struct ext2_super_block *sb, u32 grp_ind)
{
    u32 group_count = sb->s_inodes_count / sb->s_inodes_per_group;
    if(grp_ind > group_count)
        return -1;

    if(EXT2_IS_SPARSE_FEAT(sb))
        if(is_power_of(grp_ind, 3) || is_power_of(grp_ind, 5) || is_power_of(grp_ind, 7))
            return 1;

    return 0;
}


struct ext2_inode* ext2_get_inode(ext2_context_t *fs_ctx, u32 inode_ind)
{
    static struct ext2_inode inode;

    /** 
     * Inode index must be greater than 0 (inode indexing starts with 1) 
     * and less or equal maximum_inodes_per_file_system
    */
    if(inode_ind > fs_ctx->sb.s_inodes_count || inode_ind == 0)
        return NULL;    // invalid inode index

    u32 block_group_id = (inode_ind - 1) / fs_ctx->sb.s_inodes_per_group;
    u32 inode_id = (inode_ind - 1) % fs_ctx->sb.s_inodes_per_group;
    struct ext2_group_desc *l_gd = ext2_get_group_desc(fs_ctx, block_group_id);

    if(l_gd == NULL)
        return NULL;

    block_seek(fs_ctx->device, l_gd->bg_inode_table, SEEK_SET);
    lseek64(fs_ctx->device, fs_ctx->sb.s_inode_size * inode_id, SEEK_CUR);
    read(fs_ctx->device, &inode, sizeof(struct ext2_inode));
    return &inode;
}


struct ext2_group_desc* ext2_get_group_desc(ext2_context_t *fs_ctx, u32 group_ind)
{
    static struct ext2_group_desc gd;
    size_t group_tables_count = 1 + (fs_ctx->sb.s_blocks_count - 1) / fs_ctx->sb.s_blocks_per_group;

    if(group_ind > group_tables_count)
        return NULL;
    
    block_seek(fs_ctx->device, fs_ctx->sb.s_first_data_block + 1, SEEK_SET);
    lseek64(fs_ctx->device, group_ind * sizeof(struct ext2_group_desc), SEEK_CUR);
    read(fs_ctx->device, &gd, sizeof(struct ext2_group_desc));
    return &gd;
}


struct ext2_super_block *ext2_get_superblock(dev_t device, block_t block_id)
{
    static struct ext2_super_block sb;
    
    if(block_seek(device, block_id, SEEK_SET) == EXT2_BAD_BLOCK_OFFSET)
        return NULL;

    read(device, &sb, EXT2_SUPER_SIZE);
    return &sb;
}


ext2_err_t ext2_check_superblock(dev_t dev, struct ext2_super_block *sb)
{
    /* Check for a block size */

    /**
     * On Linux, kernel allowed to mount file systems only with block-size smaller
     * or equal to the system page size, hence the upper bound for a resonable
     * block-size is obtained from the operating system limits
     */
    long upper_bound = sysconf(_SC_PAGE_SIZE);
    if(upper_bound > 0)
        upper_bound %= 1024;
    else
        upper_bound = 3;        /* If PAGESIZE is not specified, guess the upper limit */
        
    if(sb->s_log_block_size > upper_bound)
        return EXT2_SUPER_BLK_ERR;

    /* Check for an inode size */
    if(sb->s_rev_level > 1)
        return EXT2_SUPER_REV_ERR;

    if(sb->s_rev_level == 0)
    {
        if(sb->s_inode_size != 128)
            return EXT2_SUPER_IND_SZ_ERR;
    }
    else
    {
        if(sb->s_inode_size > block_size)
            return EXT2_SUPER_IND_SZ_ERR;
    }

    /* Check for a blocks per group */
    if(sb->s_blocks_per_group > (block_size * 8))
        return EXT2_SUPER_BLK_PER_GRP_ERR;

    /* Check for an inodes per group */
    if(sb->s_inodes_per_group > (block_size * 8))
        return EXT2_SUPER_IND_PER_GRP_ERR;

    if((sb->s_inodes_per_group % (block_size / sb->s_inode_size)) != 0)
        return EXT2_SUPER_IND_PER_GRP_ERR;

    /* Check for a blocks count */
    u32 group_count = sb->s_inodes_count / sb->s_inodes_per_group;
    u32 total_block_count = group_count * sb->s_blocks_per_group;
    
    if(sb->s_blocks_count > total_block_count)
        return EXT2_SUPER_BLK_CNT_ERR;
    
    /* Additionally check for whether we are within device's bound */
    off_t last = lseek64(dev, 0, SEEK_END);
    off_t bts_cnt = block_size * sb->s_blocks_count; 
    if(last < bts_cnt)
        return EXT2_SUPER_BLK_CNT_ERR;

    return EXT2_NO_ERR;
}


static inline bool check_value(u32 mode, u32 flag)
{
    if((mode | flag) != flag)
        return false;
    return true;
}


ext2_err_t ext2_check_inode(struct ext2_inode *ino)
{
    /* Check for an inode mode */
    u32 mask_ino = ino->i_mode & 0xF0000;
    bool is_valid = false;

    if(check_value(mask_ino, EXT2_S_IFREG))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFSOCK))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFLNK))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFBLK))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFDIR))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFCHR))
        is_valid = true;
    else if(check_value(mask_ino, EXT2_S_IFIFO))
        is_valid = true;
    
    if(!is_valid)
        return EXT2_INO_MODE_ERR;

    return EXT2_NO_ERR;
}