/*
        Hangul code type determination moudle

        Copyright (C) 1999 by KO Myung-Hun

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Library General Public
        License as published by the Free Software Foundation; either
        version 2 of the License, or any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Library General Public License for more details.

        You should have received a copy of the GNU Library General Public
        License along with this library; if not, write to the Free
        Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Environment :

        Source file   : hct.c
        Used compiler : Borland C++ 1.0 for OS/2
                        emx 0.9d + gcc 2.8.1

    Change Log :

        Written by KO Myung-Hun
        Term of programming : 1999.02.21 - 199.02.22

*/

#include "ktype.h"
#include "hct.h"

THCTYPE queryHCT( WORD hc )
{
    BYTE first = ( hc & 0xFF00 ) >> 8;
    BYTE second = hc & 0xFF;

/*
    if( first < 0x80 )
        return UNKNOWN;
*/

    if(( first < 0x88 ) || ( first > 0xFE ))
        return UNKNOWN;

    if(( second < 0x31 ) || ( second > 0xFE ))
        return UNKNOWN;

    if((( first >= 0x88 ) && ( first < 0xA1 )) ||
       (( first > 0xAD ) && ( first < 0xB0 )) ||
       (( first > 0xC8 ) && ( first < 0xCA )))
        return JOHAB;

    if(( first > 0xF9 ) && ( first <= 0xFD ))
        return WANSUNG;

    if(( second >= 0x31 ) && ( second < 0xA1 ))
        return JOHAB;

    return UNKNOWN;
}

THCTYPE queryHCTStr( const BYTE *s )
{
    BYTE first, second;
    THCTYPE hct;

    for( ; *s != '\0'; s ++ )
    {
        if(( *s > 0x7F ) && ( *( s + 1 ) != '\0' ))
        {
            first = *s;
            second = *( s + 1 );
            hct = queryHCT(( first << 8 ) | second );
            if( hct != UNKNOWN )
                return hct;

            s ++;
        }
    }

    return UNKNOWN;
}

THCTYPE queryHCTStrL( const BYTE *s, int length )
{
    BYTE first, second;
    THCTYPE hct;
    const BYTE *limit = s + length;

    for( ; s < limit; s ++ )
    {
        if(( *s > 0x7F ) && (( s + 1 ) < limit ))
        {
            first = *s;
            second = *( s + 1 );
            hct = queryHCT(( first << 8 ) | second );
            if( hct != UNKNOWN )
                return hct;

            s ++;
        }
    }

    return UNKNOWN;
}


