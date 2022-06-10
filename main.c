#include <ext2fs.h>
#include <tree.h>
#include <math.h>

size_t block_size;

void err_sys(const char *);
void ext2_sb_print(struct ext2_super_block *);
void ext2_gd_print(struct ext2_group_desc *);
void ext2_inode_print(struct ext2_inode *);
bool is_power_of(u32, u32);

int main()
{
    int fd;
    if((fd = open64("/dev/sdb1", O_RDONLY | O_NONBLOCK)) < 0)
        err_sys("error opening device: /dev/sdb1");  // PP

    struct ext2_sb_wrap ext2_sb;
    ext2_sb.device = fd;

    // first superblock
    lseek64(fd, 1024, SEEK_SET);
    read(fd, &ext2_sb.sb, EXT2_SUPER_SIZE);
    block_size = 1024 << ext2_sb.sb.s_log_block_size;    

    if(ext2_check_superblock(&ext2_sb.sb) != 0)
        printf("INVALID SUPERBLOCK\n");

    ext2_check_superblock(&ext2_sb.sb);
    ext2_sb_print(&ext2_sb.sb);

    return 0;
}

bool is_power_of(u32 num, u32 power)
{
    if(num == 1)
        return true;

    u32 val = power;
    for(u32 i = 1; val <= num; i++)
    {
        val = pow(power, i);
        if(val == num)
            return true;
    }

    return false;
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