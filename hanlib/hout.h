#ifndef _HOUT_H
#define _HOUT_H

#if defined (__cplusplus)
extern "C" {
#endif

// hout uses 8x2x2 font only

#define HOUT_ENGFONTBUFSIZE 4096
#define HOUT_HANFONTBUFSIZE 11520
#define HOUT_SPECFONTBUFSIZE 36096
#define HOUT_HANJAFONTBUFSIZE 156416

void  HanOut(HPS hps,LONG x,LONG y,PBYTE str);
void  HanOutOfs(HPS hps,LONG x,LONG y,PBYTE str,ULONG ofs,ULONG len);
void  HanOutHch(HPS hps,LONG x,LONG y,HANCHAR hch);

ULONG houtInit(HAB hab,ULONG maxstrlen);
void  houtClose();

int houtReadEngFont(const char *filename,BYTE *buf);
int houtReadHanFont(const char *filename,BYTE *buf);
int houtReadSpecFont(const char *filename,BYTE *buf);
int houtReadHanjaFont(const char *filename,BYTE *buf);

BYTE* houtSetEngFont(BYTE* fontbuf);
BYTE* houtGetHanFont(void);

BYTE* houtSetHanFont(BYTE* fontbuf);
BYTE* houtGetHanFont(void);

BYTE* houtSetSpecFont(BYTE* fontbuf);
BYTE* houtGetSpecFont(void);

BYTE* houtSetHanjaFont(BYTE* fontbuf);
BYTE* houtGetHanjaFont(void);

#if defined (__cplusplus)
}
#endif

#endif _HOUT_H_
