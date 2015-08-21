#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "fast_file_reader.h"

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}

typedef struct sdata_t {
  long unsigned int order;
  long unsigned int freq;
  size_t key_length;
  size_t size;
} sdata;

typedef struct dynam_array_t {
  size_t size;
  size_t capacity;
  size_t nelem;
  void *data; 
} dynam_array;

typedef struct array_hash_t {
  unsigned int size;
  unsigned int order;
  dynam_array **slot;
} array_hash;

static inline size_t ah_multiple(size_t requested_size){
  size_t multiple = 16;
  return (
      ( 
       requested_size/multiple + 
       (((requested_size % multiple)>0)? 1: 0)
      ) * multiple
  );
}

array_hash *NewArrayHash(){
  array_hash *a = (array_hash *)calloc(1, sizeof(struct array_hash_t));
  assert(a);

  a->size = 65536;
  a->order = 1;
  a->slot = (dynam_array **)calloc(a->size,sizeof(struct dynam_array_t *));
  assert(a->slot);

  return a;
}

#define DYNAM_ARRAY_START_CAP 8192
static dynam_array *NewDynamArray(array_hash *a, char *str, size_t len){
  sdata *sd;
  char *scp;
  dynam_array *d;
  size_t dsize, sdsize, capacity = 0;

  sdsize = ah_multiple(sizeof(struct sdata_t) + len);
  dsize = sizeof(struct dynam_array_t) + sdsize;
  capacity = DYNAM_ARRAY_START_CAP;
  while (capacity < dsize) capacity *= 2;
  
  d = (dynam_array *)malloc(capacity);
  assert(d);

  d->capacity = capacity;
  d->size = dsize;
  d->nelem = 1;

  d->data = (void *)(d+1);
  sd = (sdata *)d->data;
  sd->size = sdsize;
  sd->order = (a->order)++;
  sd->freq = 1;
  sd->key_length = len;
  scp = (char *)(sd+1);
  memcpy((void *)scp,(void *)str,len);

  return d;
}

static dynam_array *AppendToDynamArray(array_hash *a, dynam_array *d, char *str, size_t len){
  sdata *sd;
  char *scp;
  size_t new_capacity=d->capacity, sdsize = ah_multiple(sizeof(struct sdata_t) + len);
  dynam_array *nd;

  while (sdsize > new_capacity - d->size)
    new_capacity *= DYNAM_ARRAY_START_CAP;

  if (new_capacity > d->capacity){
    nd = (dynam_array *)realloc(d,new_capacity);
    assert(nd);
    nd->data = (void *)(nd+1);
  } else {
    nd = d;
  }

  sd = (sdata *)((char *)nd + nd->size);
  sd->size = sdsize;
  sd->order = (a->order)++;
  sd->freq = 1;
  sd->key_length = len;
  scp = (char *)(sd+1);
  memcpy((void *)scp,(void *)str, len);

  (nd->nelem)++;
  nd->size += sdsize;

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
        if (sd->key_length == len && strncmp(scp,str,len)==0 ){
          (sd->freq)++;
          return;
        }
        data = (void *)sd + sd->size;
    }
  }

  a->slot[i] = (a->slot[i]) ? 
    AppendToDynamArray(a, a->slot[i],str, len) :
    NewDynamArray(a, str, len);
  return;
}

static void ClearOrderFreq(array_hash *a){
  unsigned int i;
  size_t elem;
  void *data;
  sdata *sd;

  a->order = 1;

  for (i = 0; i < a->size; i++){
    if (!a->slot[i]) continue;
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        sd->order = 0; 
        sd->freq = 0;
        data = (void *)sd + sd->size;
    }
  }
}
static void SetOrderFreq(array_hash *a, char *str, size_t len) {
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
        if (sd->key_length == len && strncmp(scp,str,len)==0 ){
          if (sd->order == 0)
            sd->order = (a->order)++;
          (sd->freq)++;
          return;
        }
        data = (void *)sd + sd->size;
    }
  }

  fprintf(stderr, "%s not in hash table!\n",str);

  return;
}

#define MAX_STRING_SIZE 8192
char input_string[MAX_STRING_SIZE];
char io_buf[MAX_STRING_SIZE];
int main(int argc, char **argv) { 
  double start, t1;
  array_hash *a = NewArrayHash();
  size_t len;
  unsigned int i, lc=0;
  size_t elem;
  sdata *sd;
  char *scp, *s;
  void *data;
  FILE *fp;
  file_buf *fb;

  start = get_wall_time();
  fb = ffr_read(argv[1]); /* DISTINCT data set. fits in memory */
  while ((s = ffr_gets(fb))!=NULL){
    HashSet(a, s, strlen(s));
  }
  ffr_free(fb);
  t1 = get_wall_time() - start;

  fprintf(stderr,"Hash built in %f secs.\n",t1);

  /* Clear frequency and order info */
  ClearOrderFreq(a);

  fprintf(stderr,"OrderFreq cleared.\n");

  start = get_wall_time();
  fp = fopen(argv[2],"r"); /* SKEW data set */
  setbuffer(fp,io_buf,MAX_STRING_SIZE);
  while (fgets( input_string, MAX_STRING_SIZE-1, fp )!=NULL){
    len = strlen(input_string) - 1; /* get rid of newline */
    SetOrderFreq(a, input_string, len);
    lc++;
    if ((lc % 1000000) == 0)
      fprintf(stderr, "lc = %u\n",lc);
  }
  fclose(fp);
  t1 = get_wall_time() - start;
  fprintf(stderr,"SKEW scanned in %f secs.\n",t1);

  for (i = 0; i < a->size; i++){
    if (!a->slot[i]) continue;
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        scp = (char *)(sd+1);
        strncpy(input_string, scp, sd->key_length);
        input_string[sd->key_length] = '\0';
        printf("%lu %lu %s\n",sd->order, sd->freq, input_string);
        data = (void *)sd + sd->size;
    }
  }
  return 0;
}
