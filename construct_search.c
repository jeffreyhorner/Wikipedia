#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static inline size_t ah_multiple(size_t requested_size){
    return ((requested_size + 7) & ~7);
}

#define SD_KEY(sd) ((char *)( (sd+1) ))
#define SD_VAL(sd) ((char *)( (char *)(sd+1) + sd->klen + 1))
typedef struct sdata_t {
  size_t size;
  size_t klen;
} sdata;

typedef struct dynam_array_t {
  size_t size;
  size_t capacity;
  size_t nelem;
  void *data; 
} dynam_array;

typedef struct array_hash_t {
  unsigned int size;
  dynam_array **slot;
} array_hash;

array_hash *NewArrayHash(){
  array_hash *a = (array_hash *)calloc(1, sizeof(struct array_hash_t));
  assert(a);

  a->size = 65536;
  a->slot = (dynam_array **)calloc(a->size,sizeof(struct dynam_array_t *));
  assert(a->slot);

  return a;
}

static long DYNAM_ARRAY_START_CAP=0;

void _init_construct_search(){
  DYNAM_ARRAY_START_CAP = sysconf(_SC_PAGESIZE) >> 1;
  fprintf(stderr,"array_size <- %lu\n",DYNAM_ARRAY_START_CAP);
}

static dynam_array *NewDynamArray(array_hash *a, 
  const char *key, size_t klen, const char *value, size_t vlen){
  sdata *sd;
  dynam_array *d;
  size_t dsize, sdsize, capacity;

  sdsize = ah_multiple(sizeof(struct sdata_t) + klen + 1 + vlen + 1);
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
  sd->klen = klen;
  memcpy((void *)SD_KEY(sd),(void *)key,klen+1);
  memcpy((void *)SD_VAL(sd),(void *)value,vlen+1);

  return d;
}

size_t NUM_REALLOCS = 0;
static dynam_array *AppendToDynamArray(array_hash *a, dynam_array *d, 
  const char *key, size_t klen, const char *value, size_t vlen){
  sdata *sd;
  size_t new_capacity=d->capacity, 
    sdsize = ah_multiple(sizeof(struct sdata_t) + klen + 1 + vlen + 1);
  dynam_array *nd;

  while (sdsize > new_capacity - d->size)
    new_capacity += DYNAM_ARRAY_START_CAP;

  if (new_capacity > d->capacity){
    NUM_REALLOCS++;
    nd = (dynam_array *)realloc(d,new_capacity);
    assert(nd);
    nd->data = (void *)(nd+1);
    nd->capacity = new_capacity;
  } else {
    nd = d;
  }

  sd = (sdata *)((char *)nd + nd->size);
  sd->size = sdsize;
  sd->klen = klen;
  memcpy((void *)SD_KEY(sd),(void *)key,klen+1);
  memcpy((void *)SD_VAL(sd),(void *)value,vlen+1);

  nd->size += sdsize;
  (nd->nelem)++;

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
static void HashSet(array_hash *a, const char *key, size_t klen, const char *value, size_t vlen) {
  unsigned int i;
  size_t elem;
  sdata *sd;
  void *data;

  i = ARRAY_SLOT(a,key,klen);
  if (a->slot[i]){
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        if (sd->klen == klen && strcmp(SD_KEY(sd),key)==0 ){
          return;
        }
        data = (void *)((char *)sd + sd->size);
    }
  }

  a->slot[i] = (a->slot[i]) ? 
    AppendToDynamArray(a, a->slot[i],key, klen, value, vlen):
    NewDynamArray(a, key, klen, value, vlen);
  return;
}

static const char *HashGet(array_hash *a, const char *key, size_t klen) {
  unsigned int i;
  size_t elem;
  sdata *sd;
  void *data;

  i = ARRAY_SLOT(a,key,klen);
  if (a->slot[i]){
    dynam_array *d = a->slot[i];
    data = d->data;
    for (elem = 0; elem < d->nelem; elem++){
        sd = (sdata *)data;
        if (sd->klen == klen && strcmp(SD_KEY(sd),key)==0 ){
          return SD_VAL(sd);
        }
        data = (void *)((char *)sd + sd->size);
    }
  }

  return NULL;
}

void HashStats( array_hash *a){
  int i;
  size_t capacity=0, size=0;

  fprintf(stderr,"hash_stats<-read.csv(file=textConnection('slot\tnelem\tcapacity\tsize\n");
  for (i = 0; i < a->size; i++){
    if (a->slot[i]){
      dynam_array *d = a->slot[i];
      if (d){
        capacity += d->capacity;
        size += d->size; 
        fprintf(stderr,"%d\t%lu\t%lu\t%lu\n",i,d->nelem,d->capacity,d->size);
      }
    }
  }
  fprintf(stderr,"'),sep='\t')\nnum_reallocs<-%lu\n",NUM_REALLOCS);
}


#define MAX_STRING_SIZE 8192
char s[MAX_STRING_SIZE];
int main(int argc, char **argv) { 
  FILE *fp;
  array_hash *a = NewArrayHash();
  const char *value = "foo";
  const char *tvalue;
  double start=0.0, t1=0.0, t2=0.0;
  unsigned int lc=0, miss = 0;

  _init_construct_search();

  fp = fopen(argv[1],"r"); /* SKEW data set */
  while (fgets( s, MAX_STRING_SIZE-1, fp )!=NULL){
    lc++;
    start = get_wall_time();
    HashSet(a, s, strlen(s), value, strlen(value));
    t1 += get_wall_time() - start;
  }
  fclose(fp);

  fp = fopen(argv[1],"r"); /* SKEW data set */
  while (fgets( s, MAX_STRING_SIZE-1, fp )!=NULL){
    start = get_wall_time();
    tvalue = HashGet(a, s, strlen(s));
    if (strcmp(tvalue,value)!=0)
      miss++;
    t2 += get_wall_time() - start;
  }
  fclose(fp);

  printf("C\t%s\t%d\t%f\t%f\n",argv[1],lc, t1, t2);

  /*HashStats(a);*/

  return 0;
}
