#ifndef EXT2_MISC_H
#define EXT2_MISC_H

#include <ext2fs.h>

/* message.c */
const char *ext2_strerror(ext2_err_t);
void ext2_sb_print(struct ext2_super_block *);
void ext2_gd_print(struct ext2_group_desc *);
void ext2_inode_print(struct ext2_inode *);

/* util.c */
bool is_power_of(u32, u32);
u8 get_bitmap_entry(struct ext2_sb_wrap *, block_t, size_t, bitmap_id);
int retain_inode(ext2_context_t *, u32);
void err_sys(const char *);

#endif // EXT2_MISC_H