#include <ext2fs.h>
#include <misc.h>
#include <directory.h>


size_t block_size;


int main()
{    
    int fd;
    if((fd = open64("/dev/sdb1", O_RDONLY | O_NONBLOCK)) < 0)
        err_sys("error opening device: /dev/sdb1");  // PP

    ext2_context_t fs_ctx;
    fs_ctx.device = fd;

    /* First superblock */
    lseek64(fd, 1024, SEEK_SET);
    read(fd, &fs_ctx.sb, EXT2_SUPER_SIZE);
    block_size = 1024 << fs_ctx.sb.s_log_block_size;    

    ext2_err_t retval = ext2_check_superblock(fs_ctx.device, &fs_ctx.sb);

    if(retval != EXT2_NO_ERR)
        printf("SUPER BLOCK ERROR: %s\n", ext2_strerror(retval));

    u32 grp_count = fs_ctx.sb.s_inodes_count / fs_ctx.sb.s_inodes_per_group;

    if((fs_ctx.inode_bitmap = bm_creat(fs_ctx.sb.s_inodes_per_group * grp_count)) == NULL)
        err_sys("can't create inode bitmap");
    
    if((fs_ctx.inode_bitmap = bm_creat(fs_ctx.sb.s_inodes_per_group * grp_count)) == NULL)
        err_sys("can't create dir bitmap");

    if((fs_ctx.inode_bitmap = bm_creat(fs_ctx.sb.s_blocks_per_group * grp_count)) == NULL)
        err_sys("can't create data bitmap");

    if((fs_ctx.grp_limits = malloc(sizeof(u32) * grp_count)) == NULL)
        err_sys("can't create group descriptor limits");

    ext2_fsck_pass1(&fs_ctx, EXT2_ROOT_DIR);

    return 0;
}