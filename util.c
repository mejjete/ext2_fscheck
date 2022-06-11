#include <ext2fs.h>
#include <math.h>

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

void set_bitmap(u8 *bitmap, u32 ind)
{
    u32 index = ind / 8;
    u32 offset = ind % 8;
    bitmap[index] &= (1 << offset);
}

void retain_inode(ext2_context_t *fs, u32 inode_ind)
{
    struct ext2_super_block *sb = &fs->sb_wrap.sb;

    u32 block_group_id = (inode_ind - 1) / sb->s_inodes_per_group;
    u32 inode_id = (inode_ind - 1) % sb->s_inodes_per_group;

    set_bitmap(fs->grp_descs[block_group_id].inode_bitmap, inode_id);
}

void err_sys(const char *msg)
{
    fprintf(stderr, "%s : %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}