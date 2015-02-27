if (require(inline, quietly=TRUE)){
  globals="
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>
  FILE *con;
  #define MAX_STRING_SIZE 7168
  char input_string[MAX_STRING_SIZE];
  "

  io <- cfunction(
    otherdefs = globals,
    sig=list(Rfopen=signature(file="character"), Rfgets=signature(), Rfclose=signature()),
    body=list(
      '
        const char *fname = CHAR(STRING_ELT(file,0));
        con = fopen(fname,"r");
        return ScalarLogical((con!=NULL)? TRUE: FALSE);
      ',
      '
        if (fgets(input_string, MAX_STRING_SIZE-1, con )!=NULL){
          input_string[strlen(input_string)-1] = 0;
          return ScalarString(mkChar(input_string));
        } else {
          return R_NilValue;
        }
      ',
      '
        fclose(con);
        return ScalarLogical(TRUE);
      '
    )
  )
  
  file <- function(fname, open="r"){
    io$Rfopen(fname)
  }
  readLines <- function(con, n=1){
    io$Rfgets()
  }
  close <- function(con){
    io$Rfclose()
  }
}
