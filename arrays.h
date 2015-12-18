#include <unistd.h>

struct array
{
	int*    v;
	size_t  cap;
	size_t  len;
};

void array_init(struct array *a, size_t capacity);
void array_add(struct array *a, int element);
void array_free(struct array *a);
void array_print(struct array* a);
void array_merge(struct array* a, struct array* aux, int lo, int mid, int hi);

/* merge top-bottom sort */
void array_msort(struct array* a, struct array* aux, int lo, int hi);

/* bottom-up merge sort */
void array_bumsort(struct array* a, struct array* aux);

/* array based heap */
void aheap_add(struct array* a, int element);
int aheap_del_max(struct array* a);

struct sarray
{
	char**  v;
	size_t  cap;
	size_t  len;
};

void sarray_init(struct sarray *a, size_t capacity);
void sarray_add(struct sarray *a, const char* element);
void sarray_free(struct sarray *a);
void sarray_print(struct sarray* a);
void sarray_merge(struct sarray* a, struct sarray* aux, int lo, int mid, int hi);

/* merge sort */
void sarray_msort(struct sarray* a, struct sarray* aux, int lo, int hi, struct sarray* ca);

/* bottom-up merge sort */
void sarray_bumsort(struct sarray* a, struct sarray* aux, struct sarray* ca);
