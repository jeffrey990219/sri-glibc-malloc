#include <stdlib.h>
#include <stdio.h>

#include "lfht.h"


uint32_t max = 16 * 4096;

uint32_t count = 8 * 4096;


static lfht_t ht;


int main(int argc, char* argv[]){
  uint32_t i;
  bool success;
  uint64_t val;

  success = init_lfht(&ht, max);

  if( !success ) exit(EXIT_FAILURE);

  for(i = 1; i <= count; i++){
    if( ! lfht_add(&ht, i * 16, i) ){
      fprintf(stderr, "%s insert failed for i = %d, max = %d\n", argv[0], i, max);
      exit(EXIT_FAILURE);
    }
  }

  for(i = 1; i <= count; i++){
    if( ! lfht_find(&ht, i * 16, &val) ){
      fprintf(stderr, "%s find failed for i = %d, max = %d\n", argv[0], i, max);
      exit(EXIT_FAILURE);
    }
    if(val != i){
      fprintf(stderr, "%s find integrity failed for i = %d, max = %d\n", argv[0], i, max);
      exit(EXIT_FAILURE);
    }
  }


  

  success = delete_lfht(&ht);

  if( !success ) exit(EXIT_FAILURE);

  fprintf(stdout, "[SUCCESS]\n");

  exit(EXIT_SUCCESS);

}

