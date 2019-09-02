#ifndef __HCH_H__
#define __HCH_H__

#ifndef NULL
#define NULL    ((void*)0)
#endif

#if defined (__cplusplus)
extern "C" {
#endif

typedef unsigned short HANCHAR;

// SY Johab

#define HCH_JSY_BASECODE            0x8441
#define HCH_JSY_FST_FILL            0x0001
#define HCH_JSY_MID_FILL            0x0002
#define HCH_JSY_LST_FILL            0x0001
#define HCH_JSY_FST_FILL_SHIFTED    0x0400
#define HCH_JSY_MID_FILL_SHIFTED    0x0040
#define HCH_JSY_LST_FILL_SHIFTED    0x0001

// KS Wansung

#define HCH_WKS_BEGIN           0xA1A1
#define HCH_WKS_HBEGIN          0xA1
#define HCH_WKS_LBEGIN          0xA1

#define HCH_WKS_SPECIAL_BEGIN   0xA1
#define HCH_WKS_SPECIAL_END     0xAC

#define HCH_WKS_HAN_BEGIN       0xB0
#define HCH_WKS_HAN_END         0xC8

#define HCH_WKS_HANJA_BEGIN     0xCA
#define HCH_WKS_HANJA_END       0xFD

#define HCH_WKS_SINGLE          0xA4
#define HCH_WKS_SINGLE_LBEGIN   0xA1
#define HCH_WKS_SINGLE_LEND     0xD3

// common

#define HCH_BASECODE            0x8441  // standard : jsy
#define HCH_FST_FILL            0x0001
#define HCH_MID_FILL            0x0002
#define HCH_LST_FILL            0x0001
#define HCH_FST_FILL_SHIFTED    0x0400
#define HCH_MID_FILL_SHIFTED    0x0040
#define HCH_LST_FILL_SHIFTED    0x0001

#define HCH_FST_MASK  0x7C00
#define HCH_MID_MASK  0x03E0
#define HCH_LST_MASK  0x001F
#define HCH_FST_INVERTED_MASK 0x83FF
#define HCH_MID_INVERTED_MASK 0xFC1F
#define HCH_LST_INVERTED_MASK 0xFFE0

#define FSTFROMHCH(hch)     (((hch)&HCH_FST_MASK)>>10)
#define MIDFROMHCH(hch)     (((hch)&HCH_MID_MASK)>>5)
#define LSTFROMHCH(hch)     (((hch)&HCH_LST_MASK))

#define HCHWITHFST(hch,fst) ((((HANCHAR)hch)&HCH_FST_INVERTED_MASK)|(((HANCHAR)fst)<<10))
#define HCHWITHMID(hch,mid) ((((HANCHAR)hch)&HCH_MID_INVERTED_MASK)|(((HANCHAR)mid)<<5))
#define HCHWITHLST(hch,lst) ((((HANCHAR)hch)&HCH_LST_INVERTED_MASK)|(((HANCHAR)lst)))

#define HCHFROM2CH(h,l)     ((HANCHAR)((((HANCHAR)h)<<8)|((unsigned char)l)))

#define HCH_SINGLE_SPACE    0x20
#define HCH_DOUBLE_SPACE    0x2020

#define ISHCH(hch)  (((HANCHAR)hch)&0x8000)

#define HCH_ENG         0x00
#define HCH_HAN         0x01
#define HCH_HAN_LEAD    0x01
#define HCH_HAN_TAIL    0x02

#define HCH_WKS         0
#define HCH_JSY         1

HANCHAR hch_getNextHch(unsigned char** pstr);
unsigned char* hch_setHchToPch(HANCHAR hch,unsigned char *pch);
unsigned char* hch_sy2ksstr(unsigned char *str);
unsigned char* hch_ks2systr(unsigned char *str);
HANCHAR hch_sy2ks(HANCHAR hch);
HANCHAR hch_ks2sy(HANCHAR hch);
HANCHAR hch_sy2ks_hanja(HANCHAR hch);
HANCHAR hch_ks2sy_hanja(HANCHAR hch);
unsigned char *hch_insertHch(unsigned char *str,HANCHAR hch);
int hch_queryHchType(unsigned char *str,int stx);
int hch_insertHchStx(unsigned char *str,HANCHAR hch,int stx);
HANCHAR hch_deleteHch(unsigned char *str);
HANCHAR hch_deleteHchStx(unsigned char *str,int stx);

int hch_decStx(unsigned char *str,int *pstx);
int hch_incStx(unsigned char *str,int *pstx);
int hch_alignStx(unsigned char *str,int *pstx);

unsigned char* hch_strlwr(unsigned char* str);

int hch_hg2hjpos( HANCHAR hg, int *pos, int *count );
HANCHAR hch_pos2hj( int n );
int hch_ishanja( HANCHAR hj );
HANCHAR hch_hj2hg( HANCHAR hj );

#if defined (__cplusplus)
}
#endif

#endif // __HCH_H__
