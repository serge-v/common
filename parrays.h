#include <unistd.h>

typedef int (*pacomp)(const void *a, const void *b);

struct parray
{
	void    **v;
	size_t  cap;
	size_t  len;
	pacomp  comp;
};

void parray_init(struct parray *a, pacomp comp);
void parray_add(struct parray *a, void *element);
void parray_free(struct parray *a);

void ppqmin_init(struct parray* a, pacomp comp);
void ppqmin_add(struct parray* a, void *element);
void *ppqmin_del(struct parray* a);
size_t ppqmin_size(struct parray* a);
