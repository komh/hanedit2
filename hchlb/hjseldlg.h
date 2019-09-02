#ifndef __HJSELDLG_H__
#define __HJSELDLG_H__

#include <os2.h>

#ifdef __cplusplus
extern "C" {
#endif

HANCHAR hjselDlg( HWND hwndParent, HWND hwndOwner, HMODULE hmod, HANCHAR hch );

MRESULT EXPENTRY hjselDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

#ifdef __cplusplus
}
#endif

#endif

