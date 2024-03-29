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
        printf("super block error: %s\n", ext2_strerror(retval));

    u32 grp_count = fs_ctx.sb.s_inodes_count / fs_ctx.sb.s_inodes_per_group;

    for(u32 i = 0; i < grp_count; i++)
    {
        retval = ext2_check_group_desc(&fs_ctx, i);
        if(retval != EXT2_NO_ERR)
            printf("%d group table descriptors error: %s\n", i, ext2_strerror(retval));
    }

    if((fs_ctx.inode_bitmap = bm_creat(fs_ctx.sb.s_inodes_per_group * grp_count)) == NULL)
        err_sys("can't create inode bitmap");
    
    if((fs_ctx.dir_bitmap = bm_creat(fs_ctx.sb.s_inodes_per_group * grp_count)) == NULL)
        err_sys("can't create directory bitmap");

    if((fs_ctx.data_bitmap = bm_creat(fs_ctx.sb.s_blocks_per_group * grp_count)) == NULL)
        err_sys("can't create data bitmap");

    if((fs_ctx.grp_ilimits = malloc(sizeof(u32) * grp_count)) == NULL)
        err_sys("can't create inode group limits");

    if((fs_ctx.grp_blimits = malloc(sizeof(u32) * grp_count)) == NULL)
        err_sys("can't create block group limits");

    /* 0th group contains reserved inodes */
    fs_ctx.grp_ilimits[0] = fs_ctx.sb.s_first_ino;
    memset(fs_ctx.grp_blimits, 0, sizeof(u32) * grp_count);

    /* Pass 1 */
    printf("Pass 1: Checking file system hierarchy\n");
    ext2_fsck_pass1(&fs_ctx, EXT2_ROOT_DIR);

    bm_release(fs_ctx.data_bitmap);
    bm_release(fs_ctx.dir_bitmap);
    bm_release(fs_ctx.inode_bitmap);
    free(fs_ctx.grp_blimits);
    free(fs_ctx.grp_ilimits);
    close(fs_ctx.device);
    return 0;
}