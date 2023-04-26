#ifndef AT__MB_IIE_GRIDVALS__DEF_H__
#define AT__MB_IIE_GRIDVALS__DEF_H__

/* quality bits */
#define MB_IIE_GRIDVALS__QUALITY_BIT_EHAT  0x00000002
#define MB_IIE_GRIDVALS__QUALITY_BIT_CV    0x00000004
#define MB_IIE_GRIDVALS__QUALITY_BIT_LNZ   0x00000008

typedef struct mb_iie_gridvals_item_t_ {
  double lnz;
  double e;
  double cv;
  unsigned quality_bits;
} mb_iie_gridvals_item_t;


typedef struct mb_iie_gridvals_t_ {
  int n;
  mb_iie_gridvals_item_t *items;
} mb_iie_gridvals_t;



#endif
