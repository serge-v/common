#include "bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int
bst_size(struct bnode* x)
{
	return (x != NULL) ? x->n : 0;
}

struct bnode*
bst_put(struct bnode* x, int val)
{
	int cmp;

	if (x == NULL)
	{
		struct bnode* n = malloc(sizeof(struct bnode));
		memset(n, 0, sizeof(struct bnode));
		n->v = val;
		n->n = 1;
		return n;
	}

	cmp = val - x->v;

	if (cmp < 0)
		x->left = bst_put(x->left,  val);
	else if (cmp > 0)
		x->right = bst_put(x->right, val);
	else
		x->v = val;

	x->n = 1 + bst_size(x->left) + bst_size(x->right);

	return x;
}

void
bst_print(struct bnode* x, int level)
{
	if (x == NULL)
		return;

	bst_print(x->left, level + 1);
	bst_print(x->right, level + 1);
	
	printf("%d ", x->v);
}

static int
height(struct bnode* n)
{
	int a, b;

	if (n == NULL)
		return 0;

	a = height(n->left);
	b = height(n->right);

	if (a > b)
		return a + 1;
	return b + 1;
}

static void
print_level(struct bnode* x, int level)
{
	if (x == NULL)
		return;

	if (level == 1)
		printf("%d ", x->v);
	else if (level > 1)
	{
		print_level(x->left, level-1);
		print_level(x->right, level-1);
	}
}

void
bst_print_level_order(struct bnode* x, int level)
{
	int i;
	int h = height(x);

	for (i = 1; i <= h; i++)
		print_level(x, i);
}

void
bst_print_tree(struct bnode* x, int level, char pos)
{
	if (x == NULL)
		return;

	printf("%*s%c %d[%d]\n", level*4, "", pos, x->v, x->n);

	bst_print_tree(x->left, level + 1, 'L');
	bst_print_tree(x->right, level + 1, 'R');
}

struct bnode*
bst_min(struct bnode* x)
{
	struct bnode* n = x;
	while (n->left != NULL)
	{
		n = n->left;
	}
	return n;
}

struct bnode*
bst_del_min(struct bnode* x)
{
	if (x->left == NULL)
		return x->right;

	x->left = bst_del_min(x->left);
	x->n = 1 + bst_size(x->left) + bst_size(x->right);

	return x;
}

struct bnode*
bst_del(struct bnode* x, int val)
{
	int cmp;
	struct bnode* t;

	if (x == NULL)
		return NULL;

	cmp = val - x->v;

	if (cmp < 0)
		x->left = bst_del(x->left, val);
	else if (cmp > 0)
		x->right = bst_del(x->right, val);
	else
	{
		if (x->right == NULL)
			return x->left;

		if (x->left == NULL)
			return x->right;

		t = x;
		x = bst_min(t->right);
		x->right = bst_del_min(t->right);
		x->left = t->left;
	}
	
	x->n = bst_size(x->left) + bst_size(x->right) + 1;
	
	return x;
}

