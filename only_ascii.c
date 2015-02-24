#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_STRING_SIZE 7168
char input_string[MAX_STRING_SIZE];
int main(void) { 
  size_t len;
  GET_STRING:
  while (fgets( input_string, MAX_STRING_SIZE-1, stdin )!=NULL){
    len = strlen(input_string) - 2; /* get rid of newline */
    do {
      if (!isprint(input_string[len--]))
        goto GET_STRING;
    } while (len>0);
    printf("%s",input_string);
  }
  return 0;
} 
