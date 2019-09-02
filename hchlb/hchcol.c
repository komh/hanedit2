#include <string.h>
#include <stdlib.h>

#include "hchcol.h"

PHCHCOL HCHColCreate( void )
{
    PHCHCOL hchCol = NULL;

    hchCol = malloc( sizeof( HCHCOL ));
    if( hchCol == NULL )
        return NULL;

    hchCol->count = 0;
    hchCol->delta = DEFAULT_DELTA;
    hchCol->limit = DEFAULT_DELTA;
    hchCol->hchBuf = malloc( hchCol->limit * ELEMENT_SIZE );
    if( hchCol->hchBuf == NULL )
    {
        free( hchCol );

        return NULL;
    }

    return hchCol;
}

void HCHColDestroy( PHCHCOL hchCol )
{
    if( hchCol == NULL )
        return;

    if( hchCol->hchBuf != NULL )
        free( hchCol->hchBuf );

    free( hchCol );
}

LONG HCHColQueryCount( PHCHCOL hchCol )
{
    if( hchCol == NULL )
        return HCHCOL_ERROR;

    return hchCol->count;
}

LONG HCHColDeleteAll( PHCHCOL hchCol )
{
    if( hchCol == NULL )
        return HCHCOL_ERROR;

    hchCol->count = 0;

    return 0;
}

LONG HCHColDelete( PHCHCOL hchCol, SHORT index )
{
    if( hchCol == NULL )
        return HCHCOL_ERROR;

    if(( index < 0 ) || ( index >= hchCol->count ))
        return HCHCOL_INDEXERROR;

    hchCol->count--;

    memmove( &hchCol->hchBuf[ index ], &hchCol->hchBuf[ index + 1 ],
             ELEMENT_SIZE * ( hchCol->count - index ));

    return hchCol->count;
}

LONG HCHColInsert( PHCHCOL hchCol, SHORT index, HANCHAR hch )
{
    if( hchCol == NULL )
        return HCHCOL_ERROR;

    if( index == HCHCOL_END )
        index = hchCol->count;

    if(( index < 0 ) || ( index > hchCol->count ))
        return HCHCOL_INDEXERROR;

    if( hchCol->count == hchCol->limit )
    {
        HANCHAR *newBuf = malloc( ELEMENT_SIZE * ( hchCol->limit + hchCol->delta ));

        if( newBuf == NULL )
            return HCHCOL_MEMERROR;

        memcpy( newBuf, hchCol->hchBuf, ELEMENT_SIZE * hchCol->limit );

        free( hchCol->hchBuf );
        hchCol->hchBuf = newBuf;
        hchCol->limit += hchCol->delta;
    }

    memmove( &hchCol->hchBuf[ index + 1 ], &hchCol->hchBuf[ index ],
             ELEMENT_SIZE * ( hchCol->count - index ));

    hchCol->hchBuf[ index ] = hch;
    hchCol->count++;

    return index;
}

HANCHAR HCHColQueryHch( PHCHCOL hchCol, SHORT index )
{
    if( hchCol == NULL )
        return HCHCOL_ERROR;

    if(( index < 0 ) || ( index >= hchCol->count ))
        return HCHCOL_INDEXERROR;

    return hchCol->hchBuf[ index ];
}

LONG HCHColSearchHch( PHCHCOL hchCol, SHORT cmd, SHORT index, HANCHAR hch )
{
    int delta = ( cmd == HCHCOL_FORWARD ) ? 1 : -1;

    if( hchCol == NULL )
        return HCHCOL_ERROR;

    if(( index < 0 ) || ( index >= hchCol->count ))
        return HCHCOL_INDEXERROR;

    while( hchCol->hchBuf[ index ] != hch )
    {
        index += delta;

        if(( index < 0 ) || ( index >= hchCol->count ))
            return HCHCOL_NONE;
    }

    return index;
}

