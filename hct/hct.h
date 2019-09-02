/*
        Header file for 'hct.c'

        Written by KO Myung-Hun
        Term of programming : 1999.02.21 - 1999.02.22

        Source file   : hct.h
        Used compiler : Borland C++ 1.0 for OS/2
                        emx 0.9d + gcc 2.8.1
*/

#ifndef _HCT_H
#define _HCT_H

#include "ktype.h"

typedef enum { UNKNOWN, WANSUNG, JOHAB } THCTYPE;

#ifdef __cplusplus
extern "C" {
#endif

THCTYPE queryHCT( WORD hr );
THCTYPE queryHCTStr( const BYTE *s );
THCTYPE queryHCTStrL( const BYTE *s, int length );

#ifdef __cplusplus
}
#endif

#endif

