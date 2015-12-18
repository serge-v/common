#include "arrays.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* integer array */

void
array_init(struct array *a, size_t capacity)
{
	a->v = (int *)malloc(capacity * sizeof(int));
	a->len = 0;
	a->cap = capacity;
}

void
array_add(struct array *a, int element)
{
	if (a->len == a->cap)
	{
		a->cap *= 2;
		a->v = (int *)realloc(a->v, a->cap * sizeof(int));
	}

	a->v[a->len++] = element;
}

void
array_free(struct array *a)
{
	free(a->v);
	a->v = NULL;
	a->len = a->cap = 0;
}

void
array_print(struct array* a)
{
	int i = 0;
	for (i = 0; i < a->len; i++)
	{
		printf(" %2d", a->v[i]);
	}
	printf("\n");
}

void
array_merge(struct array* a, struct array* aux, int lo, int mid, int hi)
{
	int i, j, k = 0;

	for (k = lo; k <= hi; k++)
		aux->v[k] = a->v[k];

	i = lo;
	j = mid + 1;
	
	for (k = lo; k <= hi; k++)
	{
		if (i > mid)
			a->v[k] = aux->v[j++];
		else if (j > hi)
			a->v[k] = aux->v[i++];
		else if (aux->v[j] < aux->v[i])
			a->v[k] = aux->v[j++];
		else
			a->v[k] = aux->v[i++];
	}
}

void
array_msort(struct array* a, struct array* aux, int lo, int hi)
{
	int mid;

	if (hi <= lo)
		return;

	mid = lo + (hi - lo) / 2;
	array_msort(a, aux, lo, mid);
	array_msort(a, aux, mid + 1, hi);
	array_merge(a, aux, lo, mid, hi);
	array_print(a);
}

void 
array_bumsort(struct array* a, struct array* aux)
{
	int sz, lo, n;

	n = a->len;
	
	if (n != aux->len)
	{
		perror("arrays lenghts are not equal");
		abort();
	}

	for (sz = 1; sz < n; sz = sz + sz)
	{
		for (lo = 0; lo < n - sz; lo += sz + sz)
		{
			int hi = lo + sz + sz - 1;
			if (n - 1 < hi)
				hi = n - 1;
			array_merge(a, aux, lo, lo + sz - 1, hi);
                        array_print(a);
		}
	}
}

/* promote element in the binary heap */
static void
swim(struct array* a, int k)
{
	while (k > 1 && a->v[k/2] < a->v[k])
	{
		/* swap element at position k with the parent at position at k/2 */
		int swap = a->v[k];
		a->v[k] = a->v[k/2];
		a->v[k/2] = swap;
		k = k / 2; /* get parent index */
	}
}

static void
sink(struct array* a, int k)
{
	int swap;
	int N = a->len - 1;

	while (2 * k <= N)
	{
		int j = 2 * k;

		if (j < N && a->v[j] < a->v[j+1])
			j++;

		if (a->v[k] >= a->v[j])
			break;

		swap = a->v[k];
		a->v[k] = a->v[j];
		a->v[j] = swap;
		k = j;
	}
}

/* add element to the binary heap */
void
aheap_add(struct array* a, int element)
{
	array_add(a, element);    /* add element to tme end */
	swim(a, a->len-1);        /* swim it up */
}

int
aheap_del_max(struct array* a)
{
	int max, N, swap;
	
	max = a->v[1];
	N = a->len - 1;

	swap = a->v[1];
	a->v[1] = a->v[N];
	a->v[N] = swap;

	a->len--;

	sink(a, 1);

	return max;
}

/* ============================ string array ====================== */

void 
sarray_init(struct sarray *a, size_t capacity)
{
	a->v = (char**)malloc(capacity * sizeof(char*));
	a->len = 0;
	a->cap = capacity;
}

void 
sarray_add(struct sarray *a, const char* element)
{
	if (a->len == a->cap)
	{
		a->cap *= 2;
		a->v = (char**)realloc(a->v, a->cap * sizeof(char*));
	}

	if (element != NULL)
	{
		a->v[a->len] = (char*)malloc(strlen(element));
		strcpy(a->v[a->len], element);
	}
	else
		a->v[a->len] = NULL;
	a->len++;
}

void 
sarray_free(struct sarray *a)
{
	free(a->v);
	a->v = NULL;
	a->len = a->cap = 0;
}

void 
sarray_print(struct sarray* a)
{
	int i = 0;
	for (i = 0; i < a->len; i++)
	{
		printf(" %s", a->v[i]);
	}
	printf("\n");
}

void 
sarray_merge(struct sarray* a, struct sarray* aux, int lo, int mid, int hi)
{
	int i, j, k = 0;

	for (k = lo; k <= hi; k++)
		aux->v[k] = a->v[k];

	i = lo;
	j = mid + 1;
	
	for (k = lo; k <= hi; k++)
	{
		if (i > mid)
			a->v[k] = aux->v[j++];
		else if (j > hi)
			a->v[k] = aux->v[i++];
		else if (strcmp(aux->v[j], aux->v[i]) < 0)
			a->v[k] = aux->v[j++];
		else
			a->v[k] = aux->v[i++];
	}
}

int 
sarray_cmp(struct sarray* a, struct sarray* b)
{
        int i;

        if (a->len > b->len)
                return b->len;
        if (b->len > a->len)
                return a->len;
        
        for (i = 0; i < a->len; i++)
        {
                if (strcmp(a->v[i], b->v[i]) != 0)
                        return i;
        }

        return -1;
}

void 
sarray_msort(struct sarray* a, struct sarray* aux, int lo, int hi, struct sarray* ca)
{
	int mid;

	if (hi <= lo)
		return;

	mid = lo + (hi - lo) / 2;
	sarray_msort(a, aux, lo, mid, ca);
	sarray_msort(a, aux, mid + 1, hi, ca);
	sarray_merge(a, aux, lo, mid, hi);
	if (sarray_cmp(a, ca) == -1)
	{
		printf("1 ");
		return;
	}
}

void 
sarray_bumsort(struct sarray* a, struct sarray* aux, struct sarray* ca)
{
	int sz, lo, n;

	n = a->len;

	if (n != aux->len)
	{
		perror("arrays lenghts are not equal");
		abort();
	}

	for (sz = 1; sz < n; sz = sz + sz)
	{
		for (lo = 0; lo < n - sz; lo += sz + sz)
		{
			int hi = lo + sz + sz - 1;
			if (n - 1 < hi)
				hi = n - 1;
			sarray_merge(a, aux, lo, lo + sz - 1, hi);
                        if (sarray_cmp(a, ca) == -1)
                        {
				printf("2 ");
				return;
			}
		}
	}
}
