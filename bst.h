/* binary search tree */

struct bnode
{
	int v;                 /* value and also a sort key */
	struct bnode* left;    /* left subtree */
	struct bnode* right;   /* right subtree */
	int n;                 /* number of nodes in subtree */
};

struct bnode* bst_put(struct bnode* x, int val);
void bst_print(struct bnode* x, int level);
void bst_print_level_order(struct bnode* x, int level);
void bst_print_tree(struct bnode* x, int level, char pos);
void bst_free(struct bnode* x);

/* find node with minimum value */
struct bnode* bst_min(struct bnode* x);

/* delete node with minimum value */
struct bnode* bst_del_min(struct bnode* x);

/* deleting using Hibbard deletion */
struct bnode* bst_del(struct bnode* x, int val);
