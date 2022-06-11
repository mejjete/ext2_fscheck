#include <ext2fs.h>
#include <misc.h>
#include <tree.h>

size_t block_size;
void set_bitmap(u8 *bitmap, u32 ind);

int main()
{
    u8 *bitmap = malloc(355);
    memset(bitmap, 0, 355);
    set_bitmap(bitmap, 9);
    printf("[%d]---[%d]---[%d]---[%d]\n", bitmap[1], bitmap[2], bitmap[3], bitmap[4]);

    return 0;
    
    
    int fd;
    if((fd = open64("/dev/sdb1", O_RDONLY | O_NONBLOCK)) < 0)
        err_sys("error opening device: /dev/sdb1");  // PP

    struct ext2_sb_wrap ext2_sb;
    ext2_sb.device = fd;

    // first superblock
    lseek64(fd, 1024, SEEK_SET);
    read(fd, &ext2_sb.sb, EXT2_SUPER_SIZE);
    block_size = 1024 << ext2_sb.sb.s_log_block_size;    

    ext2_err_t retval = ext2_check_superblock(ext2_sb.device, &ext2_sb.sb);

    if(retval != EXT2_NO_ERR)
        printf("SUPER BLOCK ERROR: %s\n", ext2_strerror(retval));
    ext2_sb_print(&ext2_sb.sb);

    /* Allocate group descriptors layout */
    u32 grp_count = ext2_sb.sb.s_inodes_count / ext2_sb.sb.s_inodes_per_group;
    ext2_context_t e2_cont;
    e2_cont.sb_wrap = ext2_sb;
    
    if((e2_cont.grp_descs = malloc(sizeof(struct ext2_grpdesc) * grp_count)) == NULL)
        err_sys("memory exhausted");

    for(u32 i = 0; i < grp_count; i++)
    {
        u8 *local_inode_bitmap;
        if((local_inode_bitmap = malloc(ext2_sb.sb.s_inodes_per_group / 8)) == NULL)
            err_sys("memory exhausted");
        
        u8 *local_data_bitmap;
        if((local_data_bitmap = malloc(ext2_sb.sb.s_blocks_per_group / 8)) == NULL)
            err_sys("memory exhausted");
        
        memset(local_inode_bitmap, 0, ext2_sb.sb.s_inodes_per_group / 8);
        memset(local_data_bitmap, 0, ext2_sb.sb.s_blocks_per_group / 8);
        
        e2_cont.grp_descs[i].free_inodes_count = 0;
        e2_cont.grp_descs[i].inode_bitmap = local_inode_bitmap;
        e2_cont.grp_descs[i].data_bitmap = local_data_bitmap;
    }

    ext2_fsck_pass1(&e2_cont);

    return 0;
}