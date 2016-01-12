#include "parrays.h"
#include <stdio.h>
#include <stdlib.h>

void
parray_init(struct parray *a, pacomp comp)
{
	a->cap = 4;
	a->v = (void **)malloc(a->cap * sizeof(void *));
	a->len = 0;
	a->comp = comp;
}

void
parray_add(struct parray *a, void *element)
{
	if (a->len == a->cap)
	{
		a->cap *= 2;
		a->v = (void **)realloc(a->v, a->cap * sizeof(void *));
	}

	a->v[a->len++] = element;
}

void
parray_free(struct parray *a)
{
	free(a->v);
	a->v = NULL;
	a->len = a->cap = 0;
}

void
parray_print(struct parray* a)
{
	int i = 0;
	for (i = 0; i < a->len; i++)
	{
		printf(" %p", a->v[i]);
	}
	printf("\n");
}

static void
swap(struct parray* a, size_t x, size_t y)
{
	void *swap = a->v[x];
	a->v[x] = a->v[y];
	a->v[y] = swap;
}

static void
swim_min(struct parray* a, int k)
{
	while (k > 1 && a->comp(a->v[k/2], a->v[k]) > 0) {
		swap(a, k, k/2); /* swap element at position k with the parent at position at k/2 */
		k = k / 2;       /* get parent index */
	}
}

static void
sink_min(struct parray* a, int k)
{
	int N = a->len - 1;

	while (2 * k <= N) {
		int j = 2 * k;

		if (j < N && a->comp(a->v[j], a->v[j+1]) > 0)
			j++;

		if (a->comp(a->v[k], a->v[j]) < 0)
			break;

		swap(a, k, j);
		k = j;
	}
}

void
ppqmin_init(struct parray* a, pacomp comp)
{
	parray_init(a, comp);
	parray_add(a, NULL);    /* add dummy zero element */
}

void
ppqmin_add(struct parray* a, void *element)
{
	parray_add(a, element);       /* add element to tme end */
	swim_min(a, a->len-1);        /* swim it up */
}

void *
ppqmin_del(struct parray* a)
{
	if (a->len <= 1)
		return NULL;

	void *min;
	size_t N;
	
	min = a->v[1];
	N = a->len - 1;
	swap(a, 1, N);
	a->len--;
	sink_min(a, 1);

	return min;
}

size_t
ppqmin_size(struct parray* a)
{
	return a->len - 1;
}
