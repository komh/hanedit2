#ifndef __SCSELDLG_H__
#define __SCSELDLG_H__

#include <os2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagSCSELINFO {
    int     category;
    int     index;
    HANCHAR hch;
} SCSELINFO, *PSCSELINFO;

HANCHAR scselDlg( HWND hwndParent, HWND hwndOwner, HMODULE hmod, PSCSELINFO pscselInfo );

MRESULT EXPENTRY scselDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

#ifdef __cplusplus
}
#endif

#endif

