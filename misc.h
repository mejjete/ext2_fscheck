#ifndef EXT2_MISC_H
#define EXT2_MISC_H


#include <ext2fs.h>


/* message.c */


const char *ext2_strerror(ext2_err_t);
void ext2_sb_print(struct ext2_super_block *);
void ext2_gd_print(struct ext2_group_desc *);
void ext2_inode_print(struct ext2_inode *);


#endif // EXT2_MISC_H