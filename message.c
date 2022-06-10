#include <ext2fs.h>

const char *ext2_strerror(ext2_err_t errcode)
{
    switch (errcode)
    {
    case 0:
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
    }
}