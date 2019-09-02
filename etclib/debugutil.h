#ifndef __debugutil_h__
#define __debugutil_h__

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>

void dumpbuf(FILE *stream,char *src);
void dump(FILE *stream,const char* name,int size,void *buf);
void dump32(FILE *stream,const char* name,int size,void *buf);

#if defined (__cplusplus)
}
#endif

#endif // __debugutil_h__
