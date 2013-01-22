#ifndef __HCHCOL_H__
#define __HCHCOL_H__

#include <os2.h>

#include "../hanlib/han.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_DELTA           25
#define ELEMENT_SIZE            ( sizeof( HANCHAR ))

#define HCHCOL_INDEXERROR       ( -3 )
#define HCHCOL_MEMERROR         ( -2 )
#define HCHCOL_ERROR            ( -1 )

#define HCHCOL_FORWARD          0
#define HCHCOL_BACKWARD         1

#define HCHCOL_FIRST            0
#define HCHCOL_END              ( -1 )

#define HCHCOL_NONE             ( -1 )

typedef struct tagHCHCOL {
    SHORT   count;
    SHORT   limit;
    SHORT   delta;
    HANCHAR *hchBuf;
} HCHCOL, *PHCHCOL;

PHCHCOL HCHColCreate( void );
void HCHColDestroy( PHCHCOL hchCol );
LONG HCHColQueryCount( PHCHCOL hchCol );
LONG HCHColDeleteAll( PHCHCOL hchCol );
LONG HCHColDelete( PHCHCOL hchCol, SHORT index );
LONG HCHColInsert( PHCHCOL hchCol, SHORT index, HANCHAR hch );
HANCHAR HCHColQueryHch( PHCHCOL hchCol, SHORT index );
LONG HCHColSearchHch( PHCHCOL hchCol, SHORT cmd, SHORT index, HANCHAR hch );

#ifdef __cplusplus
}
#endif

#endif
