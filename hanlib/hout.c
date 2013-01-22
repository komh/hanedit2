#define INCL_PM
#define INCL_GPIPRIMITIVES
#include <os2.h>
#include <stdio.h>
#include <strings.h>
#include "hch.h"
#include "hout.h"
#include "font.h"

#define X32(x)  (((ULONG)x)<<5)
#define X16(x)  (((ULONG)x)<<4)
#define X8(x)   (((ULONG)x)<<3)
#define X4(x)   (((ULONG)x)<<2)
#define X2(x)   (((ULONG)x)<<1)

#define FST_BASE 0
#define MID_BASE 160
#define LST_BASE 248
#define SPEC_BASE 0

#define WITH_LST    1
#define WITHOUT_LST 0

static const BYTE idx_fromCode[3][32]={
{0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,0,0,0,0,0,0,0,0,0,0,0},        // FST
{0,0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21,0,0},      // MID
{0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27,0,0} // LST
};

static const BYTE FST_fromMID[2][22] = {            // determine FST-font from MID
{0,0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0},      // without LST
{0,5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5}       // with LST
};

static const BYTE MID_fromFST[2][20] = {            // determine MID-font from FST
{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1},          // without LST
{2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3,3}           // with LST
};

static const BYTE LST_fromMID[22] =                 // determine LST-font from MID
{0,0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,/*2*/1,3,3,1,1};

static const SHORT blank_font[16] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static PBYTE bits=NULL;         // ----------------------------------------
static PBYTE pbits=NULL;        // global variables : for output perfomance
static ULONG line_len=0;        // ----------------------------------------

static ULONG g_maxstrlen;

BYTE *hout_internal_HanFont;
BYTE *hout_internal_EngFont;
BYTE *hout_internal_SpecFont;
BYTE *hout_internal_HanjaFont;

static void XferHanfont(BYTE *pch_head);
static void XferHanfontHead(BYTE *pch_head);
static void XferHanfontTail(BYTE *pch_head);

int houtReadEngFont(const char *filename,BYTE *buf)
{
FILE *f;

    f = fopen(filename,"r");
    if (f==NULL) return -1;
    if (fread(buf,HOUT_ENGFONTBUFSIZE,1,f)!=1)
        {
        fclose(f);
        return -1;
        }
    fclose(f);
    return 0;
}

int houtReadHanFont(const char *filename,BYTE *buf)
{
FILE *f;

    f = fopen(filename,"r");
    if (f==NULL) return -1;
    if (fread(buf,HOUT_HANFONTBUFSIZE,1,f)!=1)
        {
        fclose(f);
        return -1;
        }
    fclose(f);
    return 0;
}

int houtReadSpecFont(const char *filename,BYTE *buf)
{
FILE *f;

    f = fopen(filename,"r");
    if (f==NULL) return -1;
    if (fread(buf,HOUT_SPECFONTBUFSIZE,1,f)!=1)
        {
        fclose(f);
        return -1;
        }
    fclose(f);
    return 0;
}

int houtReadHanjaFont(const char *filename,BYTE *buf)
{
FILE *f;

    f = fopen(filename,"r");
    if (f==NULL) return -1;
    if (fread(buf,HOUT_HANJAFONTBUFSIZE,1,f)!=1)
        {
        fclose(f);
        return -1;
        }
    fclose(f);
    return 0;
}

BYTE* houtSetEngFont(BYTE* fontbuf)
{
BYTE* oldbuf = hout_internal_EngFont;

    hout_internal_EngFont = fontbuf;

    return oldbuf;
}

BYTE* houtSetHanFont(BYTE* fontbuf)
{
BYTE* oldbuf = hout_internal_HanFont;

    hout_internal_HanFont = fontbuf;

    return oldbuf;
}

BYTE* houtSetSpecFont(BYTE* fontbuf)
{
BYTE* oldbuf = hout_internal_SpecFont;

    hout_internal_SpecFont = fontbuf;

    return oldbuf;
}

BYTE* houtSetHanjaFont(BYTE* fontbuf)
{
BYTE* oldbuf = hout_internal_HanjaFont;

    hout_internal_HanjaFont = fontbuf;

    return oldbuf;
}

BYTE* houtGetEngFont(void)
{
    return hout_internal_EngFont;
}

BYTE* houtGetHanFont(void)
{
    return hout_internal_HanFont;
}

BYTE* houtGetSpecFont(void)
{
    return hout_internal_SpecFont;
}

BYTE* houtGetHanjaFont(void)
{
    return hout_internal_HanjaFont;
}


ULONG houtInit(HAB hab,ULONG maxstrlen)
{
    g_maxstrlen = maxstrlen;
    DosAllocMem((PPVOID)&bits,g_maxstrlen*16,fALLOC);
    if (bits == NULL) return FALSE;

    hout_internal_HanFont = DefaultHanFont;
    hout_internal_EngFont = DefaultEngFont;
    hout_internal_SpecFont = DefaultSpecFont;
    hout_internal_HanjaFont = NULL;

    return TRUE;
}

void houtClose()
{
    DosFreeMem(bits);
}

void  HanOut(HPS hps,LONG x,LONG y,PSZ str)
{
char *endStr = str + strlen(str) -1;
register char *pstr = str;
register int j;
register PBYTE pfont;
SIZEL sizl = {X8(strlen(str)),16};
POINTL ptl = {x,y+15};

    pbits = bits;
    line_len = strlen(str);

    while (pstr<=endStr)
        {
        if ( (*pstr) & 0x80 )
            {

            XferHanfont(pstr);
            pstr +=2;
            pbits +=2;

            } else {

            pfont = &hout_internal_EngFont[X16(*pstr)];
            for (j=0;j<16;j++,pfont++,pbits+=line_len)
                (*pbits) = *(pfont);
            pbits -= X16(line_len);
            pstr++;
            pbits++;

            }
        }

/* blitz */

    GpiMove(hps,&ptl);
    GpiImage(hps,0L,&sizl,X2(sizl.cx),bits);
}

void  HanOutOfs(HPS hps,LONG x,LONG y,PSZ str,ULONG ofs,ULONG len)
{
char *beginStr = str+ofs;
char *endStr = beginStr + len -1;
register char *pstr = str;
register int j;
register PBYTE pfont;
SIZEL sizl = {X8(len),16};
POINTL ptl = {x,y+15};

    pbits = bits;
    line_len = len;

    while (pstr<beginStr)
        if (*pstr&0x80)
            pstr+=2;
        else
            pstr++;

    if (pstr>beginStr) // *beginStr is hanTail
        {
        XferHanfontTail( beginStr-1);
        pbits ++;
        }

    while (pstr<endStr)
        {
        if ( (*pstr) & 0x80 )
            {

            XferHanfont(pstr);
            pstr +=2;
            pbits +=2;

            } else {

            pfont = &hout_internal_EngFont[X16(*pstr)];
            for (j=0;j<16;j++,pfont++,pbits+=line_len)
                (*pbits) = *(pfont);
            pbits -= X16(line_len);
            pstr++;
            pbits++;

            }
        }

    if (pstr == endStr)
        {
        if ( (*pstr) & 0x80 )
            {

            XferHanfontHead(pstr);

            } else {

            pfont = &hout_internal_EngFont[X16(*pstr)];
            for (j=0;j<16;j++,pfont++,pbits+=line_len)
                (*pbits) = *(pfont);
            pbits -= X16(line_len);
            }
        }

/* blitz */

    GpiMove(hps,&ptl);
    GpiImage(hps,0L,&sizl,X2(sizl.cx),bits);
}

void  HanOutHch(HPS hps,LONG x,LONG y,HANCHAR hch)
{
register PBYTE pfont = NULL;
register int j;
SIZEL sizl = {16,16};
POINTL ptl = {x,y+15};

    pbits = bits;
    line_len = 2;

    if ( hch & 0x8000 )
            {

            BYTE pstr[2]={HIBYTE(hch),LOBYTE(hch)};
            XferHanfont(pstr);

            } else {

            pfont = &hout_internal_EngFont[X16(hch)];
            for (j=0;j<16;j++,pfont++,pbits+=2)
                (*pbits) = *(pfont);

            }

/* blitz */

    GpiMove(hps,&ptl);
    GpiImage(hps,0L,&sizl,X2(sizl.cx),bits);
}

void  XferHanfont(BYTE *pch_head)
{
register ULONG i;
register BYTE *pfont;
register BYTE *pfont2,*pfont3;

ULONG fst,mid,lst;
ULONG idx1,idx2,idx3;
BYTE *pch_tail = (pch_head+1);
HANCHAR hch = HCHFROM2CH(*pch_head,*pch_tail);

    // special character
    if (((*pch_head) >= 0xd9) && ((*pch_head) <=0xde))
        {
        if (((*pch_tail) >= 0x31) && ((*pch_tail) <= 0x7e))
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x31);
            else
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x91+0x7f-0x31);

        pfont=&hout_internal_SpecFont[X32(idx1)];
        for(i=0;i<16;i++,pbits+=line_len)
            *((USHORT*)(pbits)) = *(((USHORT*)pfont)++);

        pbits -= X16(line_len);
        return;
        }

    // hanja character
    if (*pch_head>=0xe0)
    {
        if (hout_internal_HanjaFont == NULL)
            hch = hch_hj2hg( hch );
        else
        {
            idx1 = hch_sy2ks_hanja(hch);
            idx1=((idx1>>8)-0xca) * (0xff-0xa1) + ((idx1&0xff)-0xa1);
#ifdef DEBUG
            if (X32(idx1)>HOUT_HANJAFONTBUFSIZE)
                {
                printf("hanja out of range! hch=%X \n",idx1);
                idx1 = 0;
                }
#endif
            pfont=&hout_internal_HanjaFont[X32(idx1)];
            for (i=0;i<16;i++,pbits+=line_len)
                *((USHORT*)(pbits))  = *(((USHORT*)pfont)++);

            pbits -= X16(line_len);
            return;
        }
    }

    fst = idx_fromCode[0][FSTFROMHCH(hch)];
    mid = idx_fromCode[1][MIDFROMHCH(hch)];
    lst = idx_fromCode[2][LSTFROMHCH(hch)];

    if (lst)
        {
        idx1 = FST_BASE + FST_fromMID[WITH_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITH_LST][fst]*22 + mid;
        idx3 = LST_BASE + LST_fromMID          [mid]*28 + lst;
        } else {
        idx1 = FST_BASE + FST_fromMID[WITHOUT_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITHOUT_LST][fst]*22 + mid;
        idx3 = 0;
        }

    pfont =&hout_internal_HanFont[X32(idx1)];
    pfont2=&hout_internal_HanFont[X32(idx2)];
    pfont3=&hout_internal_HanFont[X32(idx3)];
//  if (!lst) pfont3=(PBYTE)blank_font;
    for(i=0;i<16;i++)
        {
        *((USHORT*)(pbits))  = *(((USHORT*)pfont )++);
        *((USHORT*)(pbits)) |= *(((USHORT*)pfont2)++);
        *((USHORT*)(pbits)) |= *(((USHORT*)pfont3)++);
        pbits+=line_len;
        }
    pbits -= X16(line_len);
}

void  XferHanfontHead(BYTE *pch_head)
{
register ULONG i;
register BYTE *pfont;
register BYTE *pfont2,*pfont3;

ULONG fst,mid,lst;
ULONG idx1,idx2,idx3;
BYTE *pch_tail = (pch_head+1);
HANCHAR hch = HCHFROM2CH(*pch_head,*pch_tail);

    // special character
    if (((*pch_head) >= 0xd9) && ((*pch_head) <=0xde))
        {
        if (((*pch_tail) >= 0x31) && ((*pch_tail) <= 0x7e))
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x31);
            else
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x91+0x7f-0x31);

        pfont=&hout_internal_SpecFont[X32(idx1)];
        for(i=0;i<16;i++,pbits+=line_len)
            {
            *pbits = *pfont;
            pfont+=2;
            }

        pbits -= X16(line_len);
        return;
        }

    // hanja character
    if (*pch_head>=0xe0)
    {
        if (hout_internal_HanjaFont == NULL)
            hch = hch_hj2hg( hch );
        else
        {
            idx1 = hch_sy2ks_hanja(hch);
            idx1=((idx1>>8)-0xca) * (0xff-0xa1) + ((idx1&0xff)-0xa1);
#ifdef DEBUG
            if (X32(idx1)>HOUT_HANJAFONTBUFSIZE)
                {
                printf("hanja out of range! hch=%X \n",idx1);
                idx1 = 0;
                }
#endif
            pfont=&hout_internal_HanjaFont[X32(idx1)];
            for (i=0;i<16;i++,pbits+=line_len)
                {
                *pbits = *pfont;
                pfont+=2;
                }

            pbits -= X16(line_len);
            return;
        }
    }

    fst = idx_fromCode[0][FSTFROMHCH(hch)];
    mid = idx_fromCode[1][MIDFROMHCH(hch)];
    lst = idx_fromCode[2][LSTFROMHCH(hch)];

    if (lst)
        {
        idx1 = FST_BASE + FST_fromMID[WITH_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITH_LST][fst]*22 + mid;
        idx3 = LST_BASE + LST_fromMID          [mid]*28 + lst;
        } else {
        idx1 = FST_BASE + FST_fromMID[WITHOUT_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITHOUT_LST][fst]*22 + mid;
        idx3 = 0;
        }

    pfont =&hout_internal_HanFont[X32(idx1)];
    pfont2=&hout_internal_HanFont[X32(idx2)];
    pfont3=&hout_internal_HanFont[X32(idx3)];
//  if (!lst) pfont3=(PBYTE)blank_font;
    for(i=0;i<16;i++)
        {
        *pbits  = *pfont;
        *pbits |= *pfont2;
        *pbits |= *pfont3;

        pfont  += 2;
        pfont2 += 2;
        pfont3 += 2;
        pbits  += line_len;
        }
    pbits -= X16(line_len);
}

void  XferHanfontTail(BYTE *pch_head)
{
register ULONG i;
register BYTE *pfont;
register BYTE *pfont2,*pfont3;

ULONG fst,mid,lst;
ULONG idx1,idx2,idx3;
BYTE *pch_tail = (pch_head+1);
HANCHAR hch = HCHFROM2CH(*pch_head,*pch_tail);

    // special character
    if (((*pch_head) >= 0xd9) && ((*pch_head) <=0xde))
        {
        if (((*pch_tail) >= 0x31) && ((*pch_tail) <= 0x7e))
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x31);
            else
            idx1 = ((*pch_head) - 0xd9)*(0x5e*2) + ((*pch_tail)-0x91+0x7f-0x31);

        pfont=&hout_internal_SpecFont[X32(idx1)] +1;
        for(i=0;i<16;i++,pbits+=line_len)
            {
            *pbits = *pfont;
            pfont+=2;
            }

        pbits -= X16(line_len);
        return;
        }

    // hanja character
    if (*pch_head>=0xe0)
        {
        if (hout_internal_HanjaFont == NULL)
            hch = hch_hj2hg( hch );
        else
        {
            idx1 = hch_sy2ks_hanja(hch);
            idx1=((idx1>>8)-0xca) * (0xff-0xa1) + ((idx1&0xff)-0xa1);
#ifdef DEBUG
            if (X32(idx1)>HOUT_HANJAFONTBUFSIZE)
            {
                printf("hanja out of range! hch=%X \n",idx1);
                idx1 = 0;
            }
#endif
            pfont=&hout_internal_HanjaFont[X32(idx1)] +1;
            for (i=0;i<16;i++,pbits+=line_len)
            {
                *pbits = *pfont;
                pfont+=2;
            }

            pbits -= X16(line_len);
            return;
        }
    }

    fst = idx_fromCode[0][FSTFROMHCH(hch)];
    mid = idx_fromCode[1][MIDFROMHCH(hch)];
    lst = idx_fromCode[2][LSTFROMHCH(hch)];

    if (lst)
        {
        idx1 = FST_BASE + FST_fromMID[WITH_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITH_LST][fst]*22 + mid;
        idx3 = LST_BASE + LST_fromMID          [mid]*28 + lst;
        } else {
        idx1 = FST_BASE + FST_fromMID[WITHOUT_LST][mid]*20 + fst;
        idx2 = MID_BASE + MID_fromFST[WITHOUT_LST][fst]*22 + mid;
        idx3 = 0;
        }

    pfont =&hout_internal_HanFont[X32(idx1)] +1;
    pfont2=&hout_internal_HanFont[X32(idx2)] +1;
    pfont3=&hout_internal_HanFont[X32(idx3)] +1;
//  if (!lst) pfont3=(PBYTE)blank_font;
    for(i=0;i<16;i++)
        {
        *pbits  = *pfont;
        *pbits |= *pfont2;
        *pbits |= *pfont3;

        pfont  += 2;
        pfont2 += 2;
        pfont3 += 2;
        pbits  += line_len;
        }
    pbits -= X16(line_len);
}


