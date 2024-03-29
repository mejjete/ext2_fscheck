#ifndef EXT2FS_H
#define EXT2FS_H


#include <ext2_config.h>
#include <util.h>


extern size_t block_size;


/**
 * Constants relative to the data blocks
 */
#define	EXT2_NDIR_BLOCKS		12
#define	EXT2_IND_BLOCK			EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK			(EXT2_IND_BLOCK + 1)
#define	EXT2_TIND_BLOCK			(EXT2_DIND_BLOCK + 1)
#define	EXT2_N_BLOCKS			(EXT2_TIND_BLOCK + 1)


#define EXT2_BAD_BLOCK_OFFSET	((off_t ) -1)


/**
 * Superblock constants
 */
#define EXT2_SUPER_SIZE		1024
#define EXT2_MAGIC  		0xEF53


#define EXT2_IS_SPARSE_FEAT(X) 		(((X)->s_feature_ro_compat & 0x0001) != 0)
#define EXT2_INODE_GRP_IND(S, I)	(((I) - 1) / (S).s_inodes_per_group)
#define EXT2_BLOCK_GRP_IND(S, B)	((B) / (S).s_blocks_per_group)


/**
 * Inode modes
 */
#define EXT2_S_IFSOCK 	0xC000
#define EXT2_S_IFLNK 	0xA000
#define EXT2_S_IFREG	0x8000
#define EXT2_S_IFBLK	0x6000
#define EXT2_S_IFDIR	0x4000
#define EXT2_S_IFCHR	0x2000
#define EXT2_S_IFIFO	0x1000 


#define EXT2_S_ISDIR(X)	(((X) & EXT2_S_IFDIR) == EXT2_S_IFDIR) 


/**
 * Inode controlling flags
 */
#define EXT2_INDEX_FL	0x00001000


#define EXT2_ROOT_DIR 	2


/**
 * Structure of the superblock (from ext2_fs.h)
 *
 * Packed attribute specified to simplify read-and-match the 
 * supeblock structure. Most majority of fields are redudant
 * and do not used for intergity checking
 */
struct __attribute__ ((__packed__)) ext2_super_block 
{
    __le32  s_inodes_count;		    /* Inodes count */
	__le32	s_blocks_count;		    /* Blocks count */
	__le32	s_r_blocks_count;	    /* Reserved blocks count */
	__le32	s_free_blocks_count;	/* Free blocks count */
	__le32	s_free_inodes_count;	/* Free inodes count */
	__le32	s_first_data_block;	    /* First Data Block */
	__le32	s_log_block_size;	    /* Block size */
	__le32	s_log_frag_size;	    /* Fragment size */
	__le32	s_blocks_per_group;	    /* # Blocks per group */
	__le32	s_frags_per_group;	    /* # Fragments per group */
	__le32	s_inodes_per_group;	    /* # Inodes per group */
	__le32	s_mtime;		        /* Mount time */
	__le32	s_wtime;		        /* Write time */
	__le16	s_mnt_count;		    /* Mount count */
	__le16	s_max_mnt_count;	    /* Maximal mount count */
	__le16	s_magic;		        /* Magic signature */
	__le16	s_state;		        /* File system state */
	__le16	s_errors;		        /* Behaviour when detecting errors */
	__le16	s_minor_rev_level; 	    /* minor revision level */
	__le32	s_lastcheck;		    /* time of last check */
	__le32	s_checkinterval;	    /* max. time between checks */
	__le32	s_creator_os;		    /* OS */
	__le32	s_rev_level;		    /* Revision level */
	__le16	s_def_resuid;	    	/* Default uid for reserved blocks */
	__le16	s_def_resgid;	    	/* Default gid for reserved blocks */


    /**
     * EXT2_DYNAMIC_REV Specific
     */
    __le32	s_first_ino; 				/* First non-reserved inode */
	__le16  s_inode_size; 				/* size of inode structure */
	__le16	s_block_group_nr; 			/* block group # of this superblock */
	__le32	s_feature_compat; 			/* compatible feature set */
	__le32	s_feature_incompat; 		/* incompatible feature set */
	__le32	s_feature_ro_compat; 		/* readonly-compatible feature set */
	__u8	s_uuid[16];					/* 128-bit uuid for volume */
	char	s_volume_name[16]; 			/* volume name */
	char	s_last_mounted[64]; 		/* directory where last mounted */
	__le32	s_algorithm_usage_bitmap; 	/* For compression */


	/**
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_COMPAT_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;		/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_padding1;


	/**
	 * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
	__u32	s_journal_inum;		/* inode number of journal file */
	__u32	s_journal_dev;		/* device number of journal file */
	__u32	s_last_orphan;		/* start of list of inodes to delete */
	__u32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_reserved_char_pad;
	__u16	s_reserved_word_pad;
	__le32	s_default_mount_opts;
 	__le32	s_first_meta_bg; 	/* First metablock block group */
	__u32	s_reserved[190];	/* Padding to the end of the block */
};


/**
 * Structure of a blocks group descriptor (from ext2_fs.h)
 */
struct __attribute__ ((__packed__)) ext2_group_desc
{
	__le32	bg_block_bitmap;		    /* Blocks bitmap block */
	__le32	bg_inode_bitmap;		    /* Inodes bitmap block */
	__le32	bg_inode_table;		        /* Inodes table block */
	__le16	bg_free_blocks_count;	    /* Free blocks count */
	__le16	bg_free_inodes_count;	    /* Free inodes count */
	__le16	bg_used_dirs_count;	        /* Directories count */
	__le16	bg_pad;
	__le32	bg_reserved[3];
};


/**
 * Structure of an inode on the disk (from ext2_fs.h)
 */
struct __attribute__ ((__packed__)) ext2_inode 
{
	__le16	i_mode;				/* File mode */
	__le16	i_uid;				/* Low 16 bits of Owner Uid */
	__le32	i_size;				/* Size in bytes */
	__le32	i_atime;			/* Access time */
	__le32	i_ctime;			/* Creation time */
	__le32	i_mtime;			/* Modification time */
	__le32	i_dtime;			/* Deletion Time */
	__le16	i_gid;				/* Low 16 bits of Group Id */
	__le16	i_links_count;		/* Links count */
	__le32	i_blocks;			/* Blocks count */
	__le32	i_flags;			/* File flags */
	union {
		struct {
			__le32  l_i_reserved1;
		} linux1;
		struct {
			__le32  h_i_translator;
		} hurd1;
		struct {
			__le32  m_i_reserved1;
		} masix1;
	} osd1;								/* OS dependent 1 */
	__le32	i_block[EXT2_N_BLOCKS];		/* Pointers to blocks */
	__le32	i_generation;				/* File version (for NFS) */
	__le32	i_file_acl;					/* File ACL */
	__le32	i_dir_acl;					/* Directory ACL */
	__le32	i_faddr;					/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;			/* Fragment number */
			__u8	l_i_fsize;			/* Fragment size */
			__u16	i_pad1;
			__le16	l_i_uid_high;		/* these 2 fields    */
			__le16	l_i_gid_high;		/* were reserved2[0] */
			__u32	l_i_reserved2;
		} linux2;
		struct {
			__u8	h_i_frag;			/* Fragment number */
			__u8	h_i_fsize;			/* Fragment size */
			__le16	h_i_mode_high;
			__le16	h_i_uid_high;
			__le16	h_i_gid_high;
			__le32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;			/* Fragment number */
			__u8	m_i_fsize;			/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;								/* OS dependent 2 */
};


/**
 * The new version of the directory entry.  Since EXT2 structures are
 * stored in intel byte order, and the name_len field could never be
 * bigger than 255 chars, it's safe to reclaim the extra byte for the
 * file_type field. (from ext2_fs.h)
 */
struct ext2_dir_entry_2
{
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* Name length */
	__u8	file_type;
	char	name[];			/* File name, up to EXT2_NAME_LEN */
};


typedef struct ext2_fscontext
{
	struct ext2_super_block sb;			/* Super lock instance */					
	struct bitmap *inode_bitmap;		/* Global inode bitmap */
	struct bitmap *data_bitmap;			/* Global data bitmap */
	struct bitmap *dir_bitmap;			/* Global directory bitmap */

	/**
	 * Arrray of pointers, each of which belongs to each group
	 * descriptor table entry and represents the number of 
	 * free inodes/blocks
	 */
	u32 *grp_ilimits;
	u32 *grp_blimits;
	dev_t device;					/* Device ID on which filesystem is resides */
} ext2_context_t;


/**
 * EXT2 error codes
 */
enum ext2_error_code
{
	EXT2_NO_ERR,
	EXT2_INVAL_IND_ERR,
	EXT2_INVAL_BLK_ERR,
	EXT2_MESS_ERR,

	/* Superblock error codes */
	EXT2_SUPER_BLK_SZ_ERR,
	EXT2_SUPER_IND_PER_GRP_ERR,
	EXT2_SUPER_REV_ERR,
	EXT2_SUPER_IND_SZ_ERR,
	EXT2_SUPER_BLK_PER_GRP_ERR,
	EXT2_SUPER_BLK_CNT_ERR,

	/* Inode error codes */
	EXT2_INO_MODE_ERR,

	/* Group descriptor table error codes */
	EXT2_GROUP_INO_TABLE_ERR,
	EXT2_GROUP_INO_BITMAP_ERR,
	EXT2_GROUP_DATA_BITMAP_ERR
};


typedef enum ext2_error_code ext2_err_t;


/* Befor use, block_size must be properly initialized */
static inline off_t block_seek(dev_t device, block_t block_id, int whence)
{
	return lseek64(device, block_id * block_size, whence);
};


/* ext2_fscheck.c */


/**
 * @brief Returns pointer to statically allocated inode structure 
 * and NULL if inode index is not valid
 */
struct ext2_inode* ext2_get_inode(ext2_context_t *, u32);


/**
 * @brief Returns pointer to statically allocated group descriptor table entry at given index
 * and NULL if group descriptor table index is not valid 
 */
struct ext2_group_desc* ext2_get_group_desc(ext2_context_t *, u32);


/**
 * @brief Returns pointer to statically allocated superblock structure
 * and NULL if superblock is violated
 */
struct ext2_super_block *ext2_get_superblock(dev_t, block_t);


/** @brief Checks superblock consistency */
ext2_err_t ext2_check_superblock(dev_t, struct ext2_super_block *);


/** @brief Checks group descriptor table's most vulnerable values */
ext2_err_t ext2_check_group_desc(ext2_context_t *, u32);


/**
 * @brief Checks whether the group contains superblock backup based on 
 * sparce feature. 
 * Return value:
 * 	0 - if group contains superblock
 * 	1 - if group does not contain superblock
 * -1 - invalid group index
 */
int ext2_is_grp_contains_sb(struct ext2_super_block *, u32);


/**/
ext2_err_t ext2_check_inode(struct ext2_inode *);



static inline int ext2_set_bm(ext2_context_t *fs_ctx, struct bitmap *bm, u32 index)
{
	if(index == 0)
		return -1;
	
	/* Inode index start with 1 */
	if(bm == fs_ctx->inode_bitmap || bm == fs_ctx->dir_bitmap)
		index--;
	return bm_set(bm, index);
}


static inline int ext2_get_bm(ext2_context_t *fs_ctx, struct bitmap *bm, u32 index)
{
	if(index == 0)
		return -1;
	
	if(bm == fs_ctx->inode_bitmap || bm == fs_ctx->dir_bitmap)
		index--;
	return bm_get(bm, index);
}


/* pass1.c */


/**
 * @brief Pass 1 iterates all over the inodes in the file system starting at 
 * the specified inode and gathers all information about on-disk file system
 * structure.
 * 
 * The information is packed into ext2_context_t structure to allow for a next
 * steps to use it. The information about inodes, blocks and directories location
 * are placed into bitmaps.
 * 
 * During pass 1 fsck builds next bitmaps:
 * inode_bitmap - bitmap of occupied inodes in a file system
 * data_bitmap - bitmap of occupied data blocks in a file system
 * dir_bitmamp - bitmap of directory block entries in a file system  
 * 
 * Additional information such as: number of occupied inodes/blocks are localized 
 * and packed into separate array and do not directly belong to a bitmap. 
 */
void ext2_fsck_pass1(ext2_context_t *, ino_t);


#endif // EXT2FS_H