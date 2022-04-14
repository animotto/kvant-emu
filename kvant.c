#include <stdio.h>
#include <stdlib.h>

#include "kvant.h"

void conv_to_olmp(HALFWORD halfword, BYTE *olmp) {
  olmp[0] = halfword & 0xf;
  olmp[1] = (halfword & 0xff) >> 4;
  olmp[2] = (halfword & 0x3ff) >> 8;
  olmp[3] = (halfword & 0x1fff) >> 10;
  olmp[4] = halfword >> 13;
}

void cuu_dump(CUU **cuu, MZU **mzu) {
  int i;
  BYTE *olmp;
  
  olmp = malloc(5 * sizeof(BYTE));
  
  printf("Level: %d\n\n[IP: %.4lx (%.4x)]\n",
	 (*cuu)->level, (*cuu)->ip - *mzu, *(*cuu)->ip);
  
  printf("[KOP: %x] [NOR: %x] [UTA: %x] [Smeshenie: %.2x]\n", (*cuu)->kop, (*cuu)->nor, (*cuu)->uta, (*cuu)->smeshenie);
  if ((*cuu)->uta == CUU_UTA_ROA)
    printf("[NRBA: %x] [RP: %x] [NIR: %x] [UTI: %x] [KOR: %x] [UNG: %x]\n",
	   (*cuu)->nrba, (*cuu)->rp, (*cuu)->nir, (*cuu)->uti, (*cuu)->kor, (*cuu)->ung);
  
  printf("\n");
  
  for (i = 0; i <= 8 - 1; i++) {
    conv_to_olmp((*cuu)->ron[i], olmp);
    if (i != 0 && i % 2 == 0) printf("\n");
    printf("[RON%d: %.4x (%x%x%x%x%x)] ",
	   i, (*cuu)->ron[i], olmp[4], olmp[3], olmp[2], olmp[1], olmp[0]);
  }
  
  printf("\n\n");
  
  printf("MZU levels:\n");
  for (i = 0; i <= MZU_LEVEL_SIZE * 4 - 1; i++) {
    if (i != 0 && i % 8 == 0) printf("\n");
    if (i % 8 == 0) printf("%.4x:\t", i);
    printf("%.4x ", (*mzu)[i]);
  }
  
  printf("\n\n");
  
  printf("MZU first page:\n");
  for (i = MZU_FIRST_PAGE; i <= MZU_SIZE - 1; i++) {
    if (i != MZU_FIRST_PAGE && i % 8 == 0) printf("\n");
    if (i % 8 == 0) printf("%.4x:\t", i);
    printf("%.4x ", (*mzu)[i]);
  }
  
  printf("\n");
}

void cuu_init(CUU **cuu, MZU **mzu) {
  printf("Init CUU\n");
  
  *cuu = malloc(sizeof(CUU));
  *mzu = malloc(MZU_SIZE * sizeof(MZU));
  
  (*cuu)->ron = *mzu;
  (*cuu)->ip = *mzu + MZU_FIRST_PAGE;
}

void cuu_free(CUU **cuu, MZU **mzu) {
  printf("Free CUU\n");
  
  free(*cuu);
  free(*mzu);
}

void cuu_exec(CUU **cuu, MZU **mzu) {
  BYTE *hw0, *hw1;
  HALFWORD *op_left, *op_right;
  
  (*cuu)->ron = &(*mzu)[(*cuu)->level * MZU_LEVEL_SIZE];
  
  hw0 = malloc(5 * sizeof(BYTE));
  conv_to_olmp(*(*cuu)->ip, hw0);
  (*cuu)->kop = hw0[4];
  (*cuu)->nor = hw0[3];
  (*cuu)->uta = hw0[2];
  (*cuu)->smeshenie = (hw0[1] << 4) | hw0[0];
  
  printf("Exec [%.4x] ", *(*cuu)->ip);
  
  if ((*cuu)->kop >= CUU_KOP_KON && (*cuu)->kop <= CUU_KOP_DIZ) {
    switch ((*cuu)->uta) {
    case CUU_UTA_ROA:
      hw1 = malloc(5 * sizeof(BYTE));
      conv_to_olmp(*((*cuu)->ip + 1), hw1);
      (*cuu)->nrba = hw1[4];
      (*cuu)->rp = hw1[3] >> 2;
      (*cuu)->nir = hw1[3] & 0x3;
      (*cuu)->uti = hw1[2];
      (*cuu)->kor = hw1[1];
      (*cuu)->ung = hw1[0];

      HALFWORD *op;
      switch ((*cuu)->uti) {
      case CUU_UTI_OBI:
	op = (*mzu + (*cuu)->ron[(*cuu)->nrba] + (*cuu)->smeshenie);
	break;

      case CUU_UTI_OPI:
	op = (*mzu + (*cuu)->ron[(*cuu)->nrba] + (*cuu)->smeshenie + (*cuu)->ron[(*cuu)->nir]);
	break;
      }
      
      if ((*cuu)->rp == 0) {
	op_left = &(*cuu)->ron[(*cuu)->nor];
	op_right = op;
      } else {
	op_left = op;
	op_right = &(*cuu)->ron[(*cuu)->nor];
      }
      
      (*cuu)->ip++;
      break;
      
    case CUU_UTA_SOR:
      op_left = &(*cuu)->ron[(*cuu)->nor];
      op_right = (*mzu + (*cuu)->ron[7] + (*cuu)->smeshenie);
      break;
      
    case CUU_UTA_SOP:
      op_left = (*mzu + (*cuu)->ron[7] + (*cuu)->smeshenie);
      op_right = &(*cuu)->ron[(*cuu)->nor];
      break;

    case CUU_UTA_PSR:
      op_left = &(*cuu)->ron[(*cuu)->nor];
      op_right = &(*mzu)[MZU_FIRST_PAGE + (*cuu)->smeshenie];
      break;
    }

    switch ((*cuu)->kop) {
    case CUU_KOP_KON:
      printf("KON\n");
      *op_left &= *op_right;
      break;
      
    case CUU_KOP_ASL:
      printf("ASL\n");
      *op_left += *op_right;
      break;

    case CUU_KOP_PRS:
      printf("PRS\n");
      *op_left = *op_right;
      break;

    case CUU_KOP_NEK:
      printf("NEK\n");
      *op_left ^= *op_right;
      break;

    case CUU_KOP_DIZ:
      printf("DIZ\n");
      *op_left |= *op_right;
      break;
    }
  } else {
    printf("!UNKNOWN!\n");
  }
  
  (*cuu)->ip++;
  if ((*cuu)->ip >= *mzu + MZU_SIZE)
		(*cuu)->ip = *mzu + MZU_FIRST_PAGE;
}
