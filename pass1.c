#include <ext2fs.h>
#include <misc.h>
#include <directory.h>
#include <util.h>


void ext2_fsck_pass1(ext2_context_t *fs_ctx, ino_t dir_ino)
{
    if(!fs_ctx)
        return;
    
    struct ext2_sb_wrap *sb_wrap = &fs_ctx->sb_wrap;

    ext2_DIR *dir = ext2_open_dir(fs_ctx, dir_ino);
    if(dir == NULL)
        return;

    struct ext2_dir_entry_2 *direntry;

    while((direntry = ext2_read_dir(fs_ctx, dir)) != NULL)
    {
        if(strcmp(direntry->name, ".") == 0 || 
            strcmp(direntry->name, "..") == 0)
            continue;

        struct ext2_inode *inode = ext2_get_inode(sb_wrap, direntry->inode);
        
        /* Check inode as unconnected object */
        ext2_err_t errcode = ext2_check_inode(inode);
        printf("CHECKED: %s\n", direntry->name);
        getchar();
        
        if(errcode != EXT2_NO_ERR)
            ext2_strerror(errcode);

        if(EXT2_FT_ISDIR(direntry->file_type))
        {
            if(strcmp(direntry->name, "lost+found") == 0)
                continue;

            ext2_fsck_pass1(fs_ctx, direntry->inode);
        }
    }

    ext2_close_dir(dir);
}