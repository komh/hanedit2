#ifndef __TEXTUTIL_H__
#define __TEXTUTIL_H__

#include <os2.h>
#include <stdlib.h>

#if defined (__cplusplus)
extern "C" {
#endif

int countlf(char* str);
int countcrlf(char* str);

int lf2crlf(char *dst,char *org);
int crlf2lf(char *dst,char *org);

// PSZ can be PSLF
// PSLF cannot be PSZ
typedef unsigned char* PSLF;

int isPSLF(char* str);
int PSLFstrlen(PSLF str);
PSZ PSLFtoPSZ(PSLF strLF);
PSLF PSLFstrcpy(PSLF dst,PSLF src);
PSLF PSLFstrncpy(PSLF dst,PSLF src,int count);

#if defined (__cplusplus)
}
#endif

#endif // __TEXTUTIL_H__
