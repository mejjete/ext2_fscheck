#include <tree.h>

struct tnode *create_node()
{
    struct tnode *root = malloc(sizeof(struct tnode));
    if(!root)   
        return false;

    root->data = NULL;
    root->child_list = NULL;
    root->parent = NULL;
    return root;
}

bool add_node(struct tnode *root, struct s_inode *new_item)
{
    if(!root || !new_item)
        return false;
    
    struct tnode *new_node;
    if((new_node = create_node()) == NULL)
        return false;
}