#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING_SIZE 7168
char input_string[MAX_STRING_SIZE];
int main(void) { 
  unsigned long sum, count, max, tmp;
  sum = 0; count = 0; max = 0;
  while (fgets( input_string, MAX_STRING_SIZE-1, stdin )!=NULL){
    count++;
    tmp = strlen(input_string) - 1; /* get rid of newline */
    sum += tmp;
    if (tmp > max) max = tmp;
  }
  printf("%f %lu\n",(double)sum/count, max);
  return 0;
} 
