#include <ext2fs.h>

extern size_t block_size;

struct ext2_context ext2_open_filsys(dev_t device);

u8 get_bitmap_entry(struct ext2_sb_wrap *sb_wrap, block_t block_id, size_t size, bitmap_id bm_ind)
{
    if(bm_ind > size)
        return -1;

    bm_ind--;
    u8 item;
    block_seek(sb_wrap->device, block_id, SEEK_SET);
    lseek64(sb_wrap->device, (bm_ind / 8), SEEK_CUR);
    read(sb_wrap->device, &item, 1);
    bm_ind -= 8 * (bm_ind / 8);
    return (item >> bm_ind) & 1;
}

struct ext2_inode* ext2_get_inode_entry(struct ext2_sb_wrap* sb_wrap, u32 inode_ind)
{
    static struct ext2_inode inode;

    if(inode_ind > sb_wrap->sb.s_inodes_count)
        return NULL;    // invalid inode index

    u32 block_group_id = (inode_ind - 1) / sb_wrap->sb.s_inodes_per_group;
    u32 inode_id = (inode_ind - 1) % sb_wrap->sb.s_inodes_per_group;
    struct ext2_group_desc *l_gd = ext2_get_group_desc(sb_wrap, block_group_id);

    if(l_gd == NULL)
        return NULL;

    block_seek(sb_wrap->device, l_gd->bg_inode_table, SEEK_SET);
    lseek64(sb_wrap->device, sb_wrap->sb.s_inode_size * inode_id, SEEK_CUR);
    read(sb_wrap->device, &inode, sizeof(struct ext2_inode));
    return &inode;
}

struct ext2_group_desc* ext2_get_group_desc(struct ext2_sb_wrap *sb_wrap, u32 group_ind)
{
    static struct ext2_group_desc gd;
    size_t group_tables_count = 1 + (sb_wrap->sb.s_blocks_count - 1) / sb_wrap->sb.s_blocks_per_group;

    if(group_ind > group_tables_count)
        return NULL;
    
    block_seek(sb_wrap->device, sb_wrap->sb.s_first_data_block + 1, SEEK_SET);
    lseek64(sb_wrap->device, group_ind * sizeof(struct ext2_group_desc), SEEK_CUR);
    read(sb_wrap->device, &gd, sizeof(struct ext2_group_desc));
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
    {
        /**
         * On Linux, kernel is able to mount only file systems with block-size smaller
         * or equal to the system page size, hence the upper bound for a resonable
         * block-size is get from the operating system limits
         */
        long upper_bound = sysconf(_SC_PAGE_SIZE);
        if(upper_bound > 0)
        {
            int power = 0;
            while(upper_bound > 1024)
            {
                upper_bound >>= 1;
                power++;
            }

            upper_bound = power;
        }
        else
            upper_bound = 3;        /* If PAGESIZE is not specified, guess the upper limit */
            

        if(sb->s_log_block_size > upper_bound)
            return EXT2_SUPER_BLK_ERR;
    }

    /* Check for an inode size */
    {
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
    }

    /* Check for an inodes per group */
    {
        if(sb->s_inodes_per_group > (block_size * 8))
            return EXT2_SUPER_IND_PER_GRP_ERR;
        
        if((sb->s_inodes_per_group % (block_size / sb->s_inode_size)) != 0)
            return EXT2_SUPER_IND_PER_GRP_ERR;
    }

    /* Check for a blocks per group */
    {
        if(sb->s_blocks_per_group > (block_size * 8))
            return EXT2_SUPER_BLK_PER_GRP_ERR;
    }

    /* Check for a blocks count */
    {
        u32 group_count = sb->s_inodes_count / sb->s_inodes_per_group;
        u32 total_block_count = group_count * sb->s_blocks_per_group;
        
        if(sb->s_blocks_count > total_block_count)
            return EXT2_SUPER_BLK_CNT_ERR;
        
        /* Additionally check for whether we are within device's bound */
        off_t last = lseek64(dev, 0, SEEK_END);
        off_t bts_cnt = block_size * sb->s_blocks_count; 
        if(last < bts_cnt)
            return EXT2_SUPER_BLK_CNT_ERR;
    }

    return 0;
}

void err_sys(const char *msg)
{
    fprintf(stderr, "%s : %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}