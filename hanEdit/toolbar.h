#ifndef _toolbar_h_
#define _toolbar_h_

#define WC_TOOLBAR	("App:HanEdit-Toolbar")

#define TOOLBAR_USERM_ADDITEM			(WM_USER+0)
#define TOOLBAR_USERM_SHOWITEM			(WM_USER+1)
#define TOOLBAR_USERM_ISITEMSHOWING		(WM_USER+2)
#define TOOLBAR_USERM_QUERYITEMHWND		(WM_USER+3)
#define TOOLBAR_USERM_UPDATE			(WM_USER+4)

typedef struct tagToolbarItemSpec {
	PSZ className;
	PSZ text;
	ULONG style;
	ULONG xsize;
	ULONG ysize;
	USHORT id;
} ToolbarItemSpec,*PToolbarItemSpec;

typedef struct tagToolbarCtlData {
	USHORT cb;
	ULONG itemMaxCount;
	ULONG stepsize;
} ToolbarCtlData;

BOOL EXPENTRY RegisterToolbarControl(HAB hab);


#endif // _toolbar__
