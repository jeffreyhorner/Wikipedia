#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


typedef struct sdata_t {
  long unsigned int order;
  long unsigned int freq;
  size_t len;
} sdata;

typedef struct dynam_array_t {
  size_t size;
  size_t nelem;
  void *data; 
} dynam_array;

typedef struct array_hash_t {
  unsigned int size;
  unsigned int order;
  dynam_array **slot;
} array_hash;

array_hash *NewArrayHash(){
  array_hash *a = (array_hash *)calloc(1, sizeof(struct array_hash_t));
  assert(a);

  a->size = 1024;
  a->order = 1;
  a->slot = (dynam_array **)calloc(a->size,sizeof(struct dynam_array_t *));
  assert(a->slot);

  return a;
}

static dynam_array *NewDynamArray(array_hash *a, char *str, size_t len){
  sdata *sd;
  char *scp;
  dynam_array *d;
  size_t dsize;

  dsize = sizeof(struct dynam_array_t) + sizeof(struct sdata_t) + len;
  d = (dynam_array *)malloc(dsize);
  assert(d);

  d->size = dsize;
  d->nelem = 1;

  d->data = (void *)(d+1);
  sd = (sdata *)d->data;
  sd->order = (a->order)++;
  sd->freq = 1;
  sd->len = len;
  scp = (char *)(sd+1);
  memcpy((void *)scp,(void *)str,len);

  return d;
}

static dynam_array *AppendToDynamArray(array_hash *a, dynam_array *d, char *str, size_t len){
  sdata *sd;
  char *scp;
  size_t newdsize = d->size + sizeof(struct sdata_t) + len;
  dynam_array *nd;

  nd = (dynam_array *)realloc(d,newdsize);
  assert(nd);

  nd->size = newdsize;
  (nd->nelem)++;

  nd->data = (void *)(nd+1);
  sd = (sdata *)(((char *)nd + nd->size) - (sizeof(struct sdata_t) + len));
  sd->order = (a->order)++;
  sd->freq = 1;
  sd->len = len;
  scp = (char *)(sd+1);
  memcpy((void *)scp,(void *)str, len);

  return nd;
}
static unsigned int char_hash(const char *s, int len)
{
    /* djb2 as from http://www.cse.yorku.ca/~oz/hash.html */
    char *p;
    int i;
    unsigned int h = 5381;
    for (p = (char *) s, i = 0; i < len; p++, i++)
  h = ((h << 5) + h) + (*p);
    return h;
}


#define ARRAY_SLOT(a,s,l) (char_hash(s,l)) & (a->size - 1)
static void HashSet(array_hash *a, char *str, size_t len) {
  unsigned int i;
  size_t elem;
  sdata *sd;
  char *scp;
  void *data;

  i = ARRAY_SLOT(a,str,len);
  if (a->slot[i]){
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        scp = (char *)(sd+1);
        if (sd->len == len && strncmp(scp,str,len)==0 ){
          (sd->freq)++;
          return;
        }
        data = (void *)(scp + sd->len);
    }
  }

  a->slot[i] = (a->slot[i]) ? 
    AppendToDynamArray(a, a->slot[i],str, len) :
    NewDynamArray(a, str, len);
  return;
}


#define MAX_STRING_SIZE 7168
char input_string[MAX_STRING_SIZE];
int main(void) { 
  array_hash *a = NewArrayHash();
  size_t len;
  unsigned int i;
  size_t elem;
  sdata *sd;
  char *scp;
  void *data;

  while (fgets( input_string, MAX_STRING_SIZE-1, stdin )!=NULL){
    len = strlen(input_string) - 1; /* get rid of newline */
    HashSet(a, input_string, len);
  }

  for (i = 0; i < a->size; i++){
    if (!a->slot[i]) continue;
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        scp = (char *)(sd+1);
        strncpy(input_string, scp, sd->len);
        input_string[sd->len] = '\0';
        printf("%lu %lu %s\n",sd->order, sd->freq, input_string);
        data = (void *)(scp + sd->len);
    }
  }
  return 0;
}
