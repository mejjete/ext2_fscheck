#ifndef EXT2_TREE_H
#define EXT2_TREE_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

struct tnode
{
    struct tnode *parent;
    struct tnode *child_list;
    struct s_inode *data;
};

struct s_inode
{
    struct s_inode *next;
    ino_t ino;
    mode_t mode;
};

struct tnode *create_node();
void delete_tree(struct tnode *);
bool add_node(struct tnode *, struct s_inode *);

#endif // EXT2_TREE_H