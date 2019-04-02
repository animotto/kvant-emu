#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "kvant.h"

int main(int argc, char *argv[]) {
  MZU *mzu_unit;
  CUU *cuu_unit;
  
  HALFWORD mzu_data[] = {
    0x2b04,
    0x2804, 0x9000,
    0x0000,
    0x0010,
    0x0020,
  };
  
  int i;
  
  printf("Run Kvant emulator\n");
  
  cuu_init(&cuu_unit, &mzu_unit);
  for (i = 0; i <= MZU_SIZE; i++)
    mzu_unit[i] = 0x0000;
  
  cuu_unit->ron[4] = 0x0011;
  memcpy(mzu_unit + MZU_FIRST_PAGE, mzu_data, sizeof(mzu_data));
  
  while (1) {
    char *key = readline("kvant> ");
    add_history(key);
    
    if (*key == 'q') break;
    
    if (*key == '0' || *key == '1'
	|| *key == '2' || *key == '3') {
      BYTE level = (*key - 48);
      printf("Set CUU level - %d\n", level);
      cuu_unit->level = level;
      continue;
    }
    
    if (*key == 'h' || *key == '?') {
      printf("q - Quit\n");
      printf("d - Dump CUU and MZU\n");
      printf("e - Execute one command\n");
      printf("r - Reset CUU\n");
      printf("x - Execute and dump\n");
      printf("0|1|2|3 - Set CUU level\n");
      printf("h|? - This help\n");
      continue;
    }
    
    switch (*key) {
    case 'd':
      cuu_dump(&cuu_unit, &mzu_unit);
      break;
      
    case 'e':
      cuu_exec(&cuu_unit, &mzu_unit);
      break;
      
    case 'r':
      cuu_free(&cuu_unit, &mzu_unit);
      cuu_init(&cuu_unit, &mzu_unit);
      break;
      
    case 'x':
      cuu_exec(&cuu_unit, &mzu_unit);
      cuu_dump(&cuu_unit, &mzu_unit);
      break;
      
    }
    
  }
  
  cuu_free(&cuu_unit, &mzu_unit);
  
  return(0);
}
