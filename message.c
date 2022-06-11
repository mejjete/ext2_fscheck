#include <ext2fs.h>
#include <misc.h>

const char *ext2_strerror(ext2_err_t errcode)
{
    switch (errcode)
    {
    case EXT2_NO_ERR:
        return "no error";
    case EXT2_SUPER_BLK_ERR:
        return "invalid blocks count";
    case EXT2_SUPER_IND_PER_GRP_ERR:
        return "invalid inodes per group";
    case EXT2_SUPER_REV_ERR:
        return "invalid revision";
    case EXT2_SUPER_IND_SZ_ERR:
        return "invalid inode size";
    case EXT2_SUPER_BLK_PER_GRP_ERR:
        return "invalid blocks per group";
    case EXT2_SUPER_BLK_CNT_ERR:
        return "invalid block count";
    default:
        return "no such error"; 
    }
}

void ext2_sb_print(struct ext2_super_block *sb)
{
    if(!sb)
    {
        printf("NULL");
        return;
    }

    if(sb->s_magic != EXT2_MAGIC)
        err_sys("super block currupted");

    if(sb->s_algorithm_usage_bitmap != 0)
        err_sys("compression algorithm is not implemented");

    printf("[SB] Inodes count: %u\n", sb->s_inodes_count);
    printf("[SB] Inode size: %u\n", sb->s_inode_size);
    printf("[SB] Inodes per group: %u\n", sb->s_inodes_per_group);
    printf("[SB] Free inodes: %u\n", sb->s_free_inodes_count);
    printf("[SB] First free inode: %u\n", sb->s_first_ino);
    printf("[SB] Descriptors group count: %u\n", 1 + (sb->s_blocks_count - 1) / sb->s_blocks_per_group);
    printf("[SB] Blocks per group: %u\n", sb->s_blocks_per_group);
    printf("[SB] Block size: %u\n", 1024 << sb->s_log_block_size);
    printf("[SB] Blocks count: %u\n", sb->s_blocks_count);
    printf("[SB] Free blocks: %u\n", sb->s_free_blocks_count);
    printf("[SB] Reserved blocks: %u\n", sb->s_r_blocks_count);
    printf("[SB] Revision: %u\n", sb->s_rev_level);
}

void ext2_gd_print(struct ext2_group_desc *gd)
{
    printf("[GD] Block bitmap: %u\n", gd->bg_block_bitmap);
    printf("[GD] Inode bitmap: %u\n", gd->bg_inode_bitmap);
    printf("[GD] Inode table: %u\n", gd->bg_inode_table);
    printf("[GD] Free inodes: %u\n", gd->bg_free_inodes_count);
}

void ext2_inode_print(struct ext2_inode *inode)
{
    const char *str_mode = NULL;
    if(S_ISREG(inode->i_mode))
        str_mode = "reg";
    else if(S_ISDIR(inode->i_mode))
        str_mode = "dir";
    else 
        str_mode = "undef";

    printf("[I] Size: %u\n", inode->i_size);
    printf("[I] Type: %s\n", str_mode);
    printf("[I] First block: %u\n", inode->i_block[0]);
}