#include <ext2fs.h>
#include <misc.h>
#include <directory.h>


size_t block_size;


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

    ext2_err_t retval = ext2_check_superblock(ext2_sb.device, &ext2_sb.sb);

    if(retval != EXT2_NO_ERR)
        printf("SUPER BLOCK ERROR: %s\n", ext2_strerror(retval));

    /* Allocate group descriptors layout */
    u32 grp_count = ext2_sb.sb.s_inodes_count / ext2_sb.sb.s_inodes_per_group;
    ext2_context_t e2_cont;
    e2_cont.sb_wrap = ext2_sb;
    
    if((e2_cont.grp_descs = malloc(sizeof(struct ext2_grpdesc) * grp_count)) == NULL)
        err_sys("memory exhausted");

    for(u32 i = 0; i < grp_count; i++)
    {
        struct bitmap *local_inode_bitmap = 
            bm_creat(ext2_sb.sb.s_inodes_per_group / 8);
        if(!local_inode_bitmap)
            err_sys("inode bitmap creation error");

        struct bitmap *local_data_bitmap = 
            bm_creat(ext2_sb.sb.s_blocks_per_group / 8);
        if(!local_data_bitmap)
            err_sys("data bitmap creation error");

        e2_cont.grp_descs[i].free_inodes_count = 0;
        e2_cont.grp_descs[i].inode_bitmap = *local_inode_bitmap;
        e2_cont.grp_descs[i].data_bitmap = *local_data_bitmap;
    }

    ext2_fsck_pass1(&e2_cont, 2);

    return 0;
}