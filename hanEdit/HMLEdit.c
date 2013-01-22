#define INCL_PM
#define INCL_DOSNLS
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include <os2.h>
#include <ctype.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "HMLEdit.h"
#include "statbar.h"
#include "toolbar.h"
#include "dlg.h"
#include "../hct/hct.h"

#define APPCLASS    "App:HanEdit"
#define APPTITLE    "HanEdit/2"

#define TOOLBAR_YSIZE   20
#define STATBAR_YSIZE   18

#define MIN_WORDWRAP_SIZE   10

#define DEFAULT_HAN_TYPE                (HMLE_HAN_KS)
#define DEFAULT_EOL_TYPE                (HMLE_EOL_CRLF)
#define DEFAULT_KBD_TYPE                (HAN_KBD_2)
#define DEFAULT_READONLY                (FALSE)
#define DEFAULT_AUTOINDENT              (TRUE)
#define DEFAULT_WORDWRAP                (FALSE)
#define DEFAULT_WORDWRAPSIZE            (0)
#define DEFAULT_FGCOLOR                 (CLR_WHITE)
#define DEFAULT_BGCOLOR                 (CLR_DARKBLUE)
#define DEFAULT_SELFGCOLOR              (CLR_DARKBLUE)
#define DEFAULT_SELBGCOLOR              (CLR_WHITE)
#define DEFAULT_MAXLINESIZE             (4096)
#define DEFAULT_TABSTOP                 (8)
#define DEFAULT_USE_TOOLBAR             (TRUE)
#define DEFAULT_USE_STATBAR             (TRUE)
#define DEFAULT_USE_SMARTTOOLBAR        (TRUE)
#define DEFAULT_USE_EXTERNALHANFONT     (FALSE)
#define DEFAULT_USE_EXTERNALENGFONT     (FALSE)
#define DEFAULT_USE_EXTERNALSPECFONT    (FALSE)
#define DEFAULT_USE_EXTERNALHANJAFONT   (FALSE)
#define DEFAULT_X                       (1)
#define DEFAULT_Y                       (1)
#define DEFAULT_XSIZE                   (70)
#define DEFAULT_YSIZE                   (20)

MRESULT APIENTRY ClientWndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

MRESULT mainwmChar(HWND hwnd, MPARAM mp1, MPARAM mp2);
MRESULT mainwmButton2Up( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT mainwmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT mainwmPaint(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT mainwmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 );
MRESULT mainwmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT mainwmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2);

MRESULT mainhewmInit(HWND hwnd,MPARAM mp1,MPARAM mp2);
MRESULT mainhewmCalcRect(HWND hwnd,MPARAM mp1,MPARAM mp2);

ULONG HESetTitlebarText(HWND hwnd);
ULONG HENew(HWND hwnd);
ULONG HEOpen(HWND hwnd);
ULONG HEImport( HWND hwnd );
ULONG HESave(HWND hwnd);
ULONG HESaveAs(HWND hwnd);
ULONG HEOptions(HWND hwnd);
ULONG HEExit(HWND hwnd);
ULONG HEAbout(HWND hwnd);
ULONG HEFind(HWND hwnd);
ULONG HESelectAll(HWND hwnd);
ULONG HESpecialChars( HWND hwnd );
ULONG HEHanja( HWND hwnd );
ULONG HEReloadAfterChangeHanCode(HWND hwnd);

void EnableReloadButton(HWND hwnd);
void DisableReloadButton(HWND hwnd);
void ShowEditButtons(HWND hwnd);
void HideEditButtons(HWND hwnd);

int HEFileOpenWithAutoHanCode(HWND hwnd, char *filename, int enableReload);
int HEFileOpen(HWND hwnd,char *filename,int enableReload);
int HEFileSave(HWND hwnd,char *filename);


ULONG fileRead2Buf( char *filename, char **buf );
ULONG queryHanTypeBuf( char *buf );
void extractDirname(void);

int winmain(void);
void morph(void);
void argcheck(int argc,char *argv[]);

static char *bootopen = NULL;

static int han_type = DEFAULT_HAN_TYPE;
static int eol_type = DEFAULT_EOL_TYPE;
static int kbd_type = DEFAULT_KBD_TYPE;
static int readonly = DEFAULT_READONLY;
static int fgColor = DEFAULT_FGCOLOR;
static int bgColor = DEFAULT_BGCOLOR;
static int selfgColor = DEFAULT_SELFGCOLOR;
static int selbgColor = DEFAULT_SELBGCOLOR;
static int maxLineSize = DEFAULT_MAXLINESIZE;
static int tabStop = DEFAULT_TABSTOP;
static int use_toolbar = DEFAULT_USE_TOOLBAR;
static int use_statbar = DEFAULT_USE_STATBAR;
static int use_smarttoolbar = DEFAULT_USE_SMARTTOOLBAR;
static int use_externalengfont = DEFAULT_USE_EXTERNALENGFONT;
static int use_externalhanfont = DEFAULT_USE_EXTERNALHANFONT;
static int use_externalspecfont = DEFAULT_USE_EXTERNALSPECFONT;
static int use_externalhanjafont = DEFAULT_USE_EXTERNALHANJAFONT;
static int xLeft = DEFAULT_X;
static int yBottom = DEFAULT_Y;
static int xSize = DEFAULT_XSIZE;
static int ySize = DEFAULT_YSIZE;

static BOOL autoIndent = DEFAULT_AUTOINDENT;
static BOOL markingState = FALSE;
static BOOL wordWrap = DEFAULT_WORDWRAP;
static int  wordWrapSize = DEFAULT_WORDWRAPSIZE;

static char szFullPath[CCHMAXPATH];
static char szDir[CCHMAXPATH];

HWND        hwndHIA = NULLHANDLE;
HWND        hwndHMLE = NULLHANDLE;
HWND        hwndStatbar = NULLHANDLE;
HWND        hwndToolbar = NULLHANDLE;
HWND        hwndPopupMenu = NULLHANDLE;

static char external_engfontbuf[HOUT_HANFONTBUFSIZE];
static char external_hanfontbuf[HOUT_HANFONTBUFSIZE];
static char external_specfontbuf[HOUT_SPECFONTBUFSIZE];
static char external_hanjafontbuf[HOUT_HANJAFONTBUFSIZE];

#define COLORINDEXNUM   16

static const char *pszColorName[COLORINDEXNUM] = {
    "black","blue","red","pink","green","cyan","yellow","lightgray",
    "darkgray","darkblue","darkred","darkpink","darkgreen","darkcyan","darkyellow","white"
};
static long colorIdx[COLORINDEXNUM] = {
    CLR_BLACK,CLR_BLUE,CLR_RED,CLR_PINK,CLR_GREEN,CLR_CYAN,CLR_YELLOW,CLR_PALEGRAY,
    CLR_DARKGRAY,CLR_DARKBLUE,CLR_DARKRED,CLR_DARKPINK,CLR_DARKGREEN,CLR_DARKCYAN,CLR_BROWN,CLR_WHITE
};


long str2clr(const char *colorname)
{
char str[256];
int i;

    strcpy(str,colorname);
    strlwr(str);
    for (i=0;i<COLORINDEXNUM;i++)
        if (strcmp(str,pszColorName[i])==0)
            return colorIdx[i];
    return CLR_BLACK;
}

void argcheck(int argc,char *argv[])
{
int idx = 1;
char str[100];
char hanfontpath[256];
char engfontpath[256];
char specfontpath[256];
char hanjafontpath[256];

    bootopen = NULL;

#ifdef DEBUG
    printf("program path = %s\n",argv[0]);
#endif
    while (idx<argc)
        {
        strcpy(str,argv[idx]);
        if (strcmp(str,"-w")==0)
            han_type = HMLE_HAN_KS;
        else if (strcmp(str,"-j")==0)
            han_type = HMLE_HAN_SY;
        else if (strcmp(str,"-readonly")==0)
            readonly=TRUE;
        else if (strcmp(str,"-no-readonly")==0)
            readonly=FALSE;
        else if (strcmp(str,"-kbd2")==0)
            kbd_type=HAN_KBD_2;
        else if (strcmp(str,"-kbd390")==0)
            kbd_type=HAN_KBD_390;
        else if (strcmp(str,"-kbd3f")==0)
            kbd_type=HAN_KBD_3FINAL;
        else if (strcmp(str,"-crlf")==0)
            eol_type = HMLE_EOL_CRLF;
        else if (strcmp(str,"-lf")==0)
            eol_type = HMLE_EOL_LF;
        else if (strcmp(str,"-toolbar")==0)
            use_toolbar = TRUE;
        else if (strcmp(str,"-no-toolbar")==0)
            use_toolbar = FALSE;
        else if (strcmp(str,"-statbar")==0)
            use_statbar = TRUE;
        else if (strcmp(str,"-no-statbar")==0)
            use_statbar = FALSE;
        else if (strcmp(str,"-smarttb")==0)
            use_smarttoolbar = TRUE;
        else if (strcmp(str,"-no-smarttb")==0)
            use_smarttoolbar = FALSE;
        else if (strcmp(str,"-hfont")==0)
            {
            use_externalhanfont = TRUE;
            idx++;
            strcpy(hanfontpath,argv[idx]);
            }
        else if (strcmp(str,"-efont")==0)
            {
            use_externalengfont = TRUE;
            idx++;
            strcpy(engfontpath,argv[idx]);
            }
        else if (strcmp(str,"-specfont")==0)
            {
            use_externalspecfont = TRUE;
            idx++;
            strcpy(specfontpath,argv[idx]);
            }
        else if (strcmp(str,"-hanjafont")==0)
            {
            use_externalhanjafont = TRUE;
            idx++;
            strcpy(hanjafontpath,argv[idx]);
            }
        else if (strcmp(str,"-fg")==0)
            {
            idx++;
            fgColor = str2clr(argv[idx]);
            selbgColor = fgColor;
            }
        else if (strcmp(str,"-bg")==0)
            {
            idx++;
            bgColor = str2clr(argv[idx]);
            selfgColor = bgColor;
            }
        else if (strcmp(str,"-cp")==0)
            {
            int cp = atoi(argv[++idx]);
            DosSetProcessCp(cp);
            }
        else if (strcmp(str,"-linebuf")==0)
            {
            maxLineSize = atoi(argv[++idx])+1;
            if (maxLineSize<16) maxLineSize = 16;
            }
        else if (strcmp(str,"-tabsize")==0)
            {
            tabStop = atoi(argv[++idx]);
            if (tabStop<1) tabStop = 1;
            }
        else if (strcmp(str,"-x")==0)
            xLeft = atoi(argv[++idx]);
        else if (strcmp(str,"-y")==0)
            yBottom = atoi(argv[++idx]);
        else if (strcmp(str,"-col")==0)
            xSize = atoi(argv[++idx]);
        else if (strcmp(str,"-line")==0)
            ySize = atoi(argv[++idx]);
        else if( strcmp( str, "-autoindent") == 0 )
            autoIndent = TRUE;
        else if( strcmp( str, "-no-autoindent") == 0 )
            autoIndent = FALSE;
        else if( strcmp( str, "-wordwrap" ) == 0 )
        {
            wordWrap = TRUE;
            wordWrapSize = atoi( argv[ ++idx ]);
        }
        else {
        bootopen = argv[idx];
        }
        idx++;
        } //while

    if (use_externalengfont)
        if (houtReadEngFont(engfontpath,external_engfontbuf)==-1)
            use_externalengfont = FALSE;
    if (use_externalhanfont)
        if (houtReadHanFont(hanfontpath,external_hanfontbuf)==-1)
            use_externalhanfont = FALSE;
    if (use_externalspecfont)
        if (houtReadSpecFont(specfontpath,external_specfontbuf)==-1)
            use_externalspecfont = FALSE;
    if (use_externalhanjafont)
        if (houtReadHanjaFont(hanjafontpath,external_hanjafontbuf)==-1)
            {
            use_externalhanjafont = FALSE;
            printf("err hanja font file\n");
            }
}


int main(int argc, char *argv[], char *envp[])
{
    _envargs(&argc,&argv,"HANEDIT2OPT");
    argcheck(argc,argv);
#ifdef DEBUG
    morph();
#endif
    winmain();

    return 0;
}

#ifdef DEBUG
void morph(void)
{
PPIB ppib = NULL;
PTIB ptib = NULL;
APIRET rc;

    rc = DosGetInfoBlocks(&ptib,&ppib);
    if (rc != NO_ERROR)
        {
        printf("DosGetInfoBlocks Error : %d\n",rc);
        exit(-1);
        }
    ppib->pib_ultype = 3;
}
#endif

int winmain()
{
HAB   hab = NULLHANDLE;
HMQ   hmq = NULLHANDLE;
QMSG  qmsg;
HWND  hwndFrame = NULLHANDLE;
HWND  hwndClient = NULLHANDLE;
ULONG ctlData = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
        FCF_TASKLIST | FCF_MENU | FCF_ACCELTABLE | FCF_NOBYTEALIGN;
RECTL rectl;

    do  {
        if ((hab = WinInitialize(0)) == NULLHANDLE) break;
        if ((hmq = WinCreateMsgQueue(hab,0)) == NULLHANDLE) break;
        if (!WinRegisterClass(hab, APPCLASS,
            ClientWndProc, CS_SIZEREDRAW, 4)) break;

        // Register the HanMLE and HanInputAutomata window class
        if (!RegisterHanAutomataClass(hab)) break;
        if (!RegisterHanMLEControl(hab)) break;
        if (!RegisterHanStaticTextControl(hab)) break;
        if (!RegisterHanEntryFieldControl(hab)) break;
        if (!RegisterStatbarControl(hab)) break;
        if (!RegisterToolbarControl(hab)) break;
        if ((hwndFrame = WinCreateStdWindow(
                    HWND_DESKTOP, 0,
                    &ctlData,
                    APPCLASS,
                    APPTITLE,
                    0, NULLHANDLE,
                    ID_APP,
                    &hwndClient )) == NULLHANDLE )
            {
            printf("errorcode: %x\n",WinGetLastError(hab));
            break;
            }
        //printf("Frame: %x\nClient: %x\n",hwndFrame,hwndClient);

        rectl.xLeft = 0;
        rectl.yBottom = 0;
        rectl.xRight = xSize;
        rectl.yTop = ySize;
        WinSendMsg(hwndClient,HEWM_CALCRECT,(MPARAM)&rectl,0);
        WinCalcFrameRect(hwndFrame,&rectl,FALSE);

        WinSetWindowPos(hwndFrame,NULLHANDLE,
                xLeft,yBottom,
                rectl.xRight-rectl.xLeft,rectl.yTop-rectl.yBottom,
                SWP_MOVE|SWP_SIZE|SWP_SHOW);
        WinSetFocus(HWND_DESKTOP,hwndFrame);

        houtInit(hab,maxLineSize);
        if (use_externalengfont)
            houtSetEngFont(external_engfontbuf);
        if (use_externalhanfont)
            houtSetHanFont(external_hanfontbuf);
        if (use_externalspecfont)
            houtSetSpecFont(external_specfontbuf);
        if (use_externalhanjafont)
            houtSetHanjaFont(external_hanjafontbuf);

        while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
            {
            WinDispatchMsg(hab, &qmsg);
            }

        houtClose();

        } while (FALSE);

    if (hmq != NULLHANDLE) WinDestroyMsgQueue(hmq);
    if (hab != NULLHANDLE) WinTerminate(hab);

    return 0;
}


MRESULT APIENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_ERASEBACKGROUND:    return MRFROMLONG(FALSE);
    case WM_COMMAND:            return mainwmCommand(hwnd,mp1,mp2);
    case WM_CONTROL:            return mainwmControl(hwnd, mp1, mp2);
    case WM_CREATE:             return mainwmCreate(hwnd, mp1, mp2);
//    case WM_SETFOCUS:           return mainwmSetFocus(hwnd,mp1,mp2);
    case WM_SIZE:               return mainwmSize(hwnd, mp1, mp2);
    case WM_CLOSE:              return mainwmClose(hwnd,mp1,mp2);
    case WM_CHAR:               return mainwmChar(hwnd,mp1,mp2);
    case WM_BUTTON2UP:          return mainwmButton2Up( hwnd, mp1, mp2 );
    case WM_TIMER:              return mainwmTimer(hwnd,mp1,mp2);

    case HEWM_INIT:             return mainhewmInit(hwnd,mp1,mp2);
    case HEWM_CALCRECT:         return mainhewmCalcRect(hwnd,mp1,mp2);
    default:                    return WinDefWindowProc(hwnd, msg, mp1, mp2);
    } /* endswitch */
}

MRESULT mainhewmCalcRect(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
PRECTL rectl = mp1;
POINTL p;

    p.x = rectl->xRight;
    p.y = rectl->yTop;
    WinSendMsg(hwndHMLE,WM_CALCFRAMERECT,(MPARAM)&p,0);
    rectl->xRight = rectl->xLeft + p.x;
    rectl->yTop = rectl->yBottom + p.y + TOOLBAR_YSIZE + STATBAR_YSIZE;

    return 0;
}

MRESULT mainhewmInit(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
HWND    hwndFrame = WinQueryWindow(hwnd,QW_PARENT);

//  printf("HEWM_INIT\n");

    if (bootopen)
        han_type = HEFileOpenWithAutoHanCode(hwnd,bootopen,TRUE);

    HESetTitlebarText(hwnd);

    WinPostMsg(hwndHIA,HIAM_SETKBDTYPE,MPFROMLONG(kbd_type),0L);
    WinPostMsg(hwndHMLE,HMLM_SETTEXTFORMAT,MPFROMLONG(eol_type),0L);
    WinPostMsg(hwndStatbar,STATBAR_USERM_SETEOLTYPE,MPFROMLONG(eol_type),0L);
    WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(han_type),0L);

    if (readonly)
        {
        WinEnableMenuItem(WinWindowFromID(hwndFrame,FID_MENU),IDM_IMPORT,FALSE );
        WinEnableMenuItem(WinWindowFromID(hwndFrame,FID_MENU),IDM_SAVE,FALSE);
        //WinEnableMenuItem(WinWindowFromID(hwndFrame,FID_MENU),IDM_SAVEAS,FALSE);
        WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_SAVE),FALSE);
        WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CUT),FALSE);
        WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_PASTE),FALSE);
        WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CLEAR),FALSE);
        WinPostMsg(hwndToolbar,TOOLBAR_USERM_UPDATE,0,0);
        }
    WinEnableMenuItem(WinWindowFromID(hwndFrame,FID_MENU),IDM_PROPERTIES,FALSE);

    WinSetFocus(HWND_DESKTOP,hwndHMLE);

    return 0L;
}

MRESULT mainwmCreate( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
PCREATESTRUCT pcreate = (PCREATESTRUCT) mp2;
HMLECTLDATA hmlecd;
ULONG flStyle = WS_VISIBLE | HMLS_VSCROLL | HMLS_HSCROLL | HMLS_STATUSNOTIFY;// | HMLS_BORDER;
//HSTCTLDATA hstcd;

ToolbarItemSpec itemSpec;
HWND hwndItem;
char text[100];
LONG color;

//  printf("HMLEdit:: WM_CREATE\n");

    if (readonly)
        {
        flStyle |=HMLS_READONLY;
        }

    if( wordWrap )
        flStyle |= HMLS_WORDWRAP;

    hwndHIA = HIACreateHanAutomata(hwnd,ID_HIA);

    hmlecd.cb = sizeof(hmlecd);
    hmlecd.eol_type = eol_type;
    hmlecd.han_type = han_type;
    hmlecd.autoIndent = autoIndent;
    hmlecd.maxLineSize = maxLineSize;
    hmlecd.wordWrapSize = wordWrapSize;
    hmlecd.hwndHIA = hwndHIA;
//  printf("cx = %d\ncy = %d\n",pcreate->cx,pcreate->cy);
//  printf("maxLineSize = %d\n",maxLineSize);

    hwndHMLE = WinCreateWindow (
        hwnd,WC_HMLE,
        NULL,
        flStyle | HMLS_ACCEPTFILEDROP,
        0,(use_statbar?STATBAR_YSIZE:0),pcreate->cx,pcreate->cy-(use_statbar?STATBAR_YSIZE:0)-(use_toolbar?TOOLBAR_YSIZE:0),
        hwnd,
        HWND_TOP,
        ID_HANMLE,
        &hmlecd,
        NULL);
    WinSendMsg(hwndHMLE,HMLM_SETTEXTCOLOR,MPFROMLONG(fgColor),0);
    WinSendMsg(hwndHMLE,HMLM_SETBACKCOLOR,MPFROMLONG(bgColor),0);
    WinSendMsg(hwndHMLE,HMLM_SETSELTEXTCOLOR,MPFROMLONG(selfgColor),0);
    WinSendMsg(hwndHMLE,HMLM_SETSELBACKCOLOR,MPFROMLONG(selbgColor),0);
    WinSendMsg(hwndHMLE,HMLM_SETTABSTOP,MPFROMLONG(tabStop),0);
//  printf("HMLE:%x\n",hwndHMLE);

//  CONTROL     "by ¹®À¯¼º", 104, 37, 60, 72, 8, "HanStaticText",SS_TEXT|DT_VCENTER|DT_CENTER|WS_VISIBLE


//  toolbar
        {
        hwndToolbar = WinCreateWindow (
            hwnd,WC_TOOLBAR,
            NULL,WS_VISIBLE,
            0,pcreate->cy-TOOLBAR_YSIZE,pcreate->cx,TOOLBAR_YSIZE,
            hwnd,HWND_TOP,
            ID_TOOLBAR,
            NULL,NULL);

        itemSpec.text = text;
        itemSpec.className = WC_HST;
        strcpy(text,"HanEdit/2");
        itemSpec.style = SS_TEXT | DT_CENTER | DT_VCENTER;
        itemSpec.xsize = 100;
        itemSpec.ysize = 0;
        itemSpec.id = -1;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {
            WinSendMsg(hwndItem,HSTM_SETTEXTCOLOR,MPFROMLONG(CLR_BLACK),0);
            } else printf("error! WC_HST\n");

        itemSpec.className = WC_BUTTON;
        strcpy(text,"Open");
        itemSpec.style = BS_PUSHBUTTON|BS_TEXT|BS_NOBORDER|BS_AUTOSIZE;
        itemSpec.xsize = -1;
        itemSpec.ysize = 0;
        itemSpec.id = IDM_OPEN;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        strcpy(text,"Save");
        itemSpec.id = IDM_SAVE;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        strcpy(text,"Change");
        itemSpec.id = IDM_QUICKRELOAD;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {
            color = CLR_RED;
            WinSetPresParam(hwndItem,PP_FOREGROUNDCOLORINDEX,sizeof(LONG),&color);
            WinSendMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_RELOAD),FALSE);
            } else printf("error!\n");

        strcpy(text,"Exit");
        itemSpec.id = IDM_EXIT;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {
            color = CLR_RED;
            WinSetPresParam(hwndItem,PP_BACKGROUNDCOLORINDEX,sizeof(LONG),&color);
            } else printf("error!\n");

        itemSpec.className = NULL;
        itemSpec.xsize = 4;
        itemSpec.id = -1;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);

        itemSpec.className = WC_BUTTON;
        strcpy(text,"Copy");
        itemSpec.xsize = -1;
        itemSpec.id = IDM_COPY;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        strcpy(text,"Cut");
        itemSpec.id = IDM_CUT;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        strcpy(text,"Paste");
        itemSpec.id = IDM_PASTE;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        strcpy(text,"Clear");
        itemSpec.id = IDM_CLEAR;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {} else printf("error!\n");

        itemSpec.className = NULL;
        itemSpec.xsize = 4;
        itemSpec.id = -1;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
/*
        itemSpec.className = WC_HEF;
        strcpy(text,"HEF");
        itemSpec.style = WS_VISIBLE|HES_BORDER;
        itemSpec.xsize = 200;
        itemSpec.ysize = 18;
        itemSpec.id = -1;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
        if (hwndItem)
            {
            WinSendMsg(hwndItem,HEM_SETTEXTCOLOR,MPFROMLONG(CLR_CYAN),0);
            WinSendMsg(hwndItem,HEM_SETBACKCOLOR,MPFROMLONG(SYSCLR_BACKGROUND),0);
            WinSendMsg(hwndItem,HEM_SETSELTEXTCOLOR,MPFROMLONG(selfgColor),0);
            WinSendMsg(hwndItem,HEM_SETSELBACKCOLOR,MPFROMLONG(selbgColor),0);
            } else printf("error!\n");

        itemSpec.className = NULL;
        itemSpec.xsize = 4;
        itemSpec.id = -1;
        hwndItem = (HWND)WinSendMsg(hwndToolbar,TOOLBAR_USERM_ADDITEM,&itemSpec,NULL);
*/
        HideEditButtons(hwnd);
        }

    hwndStatbar = WinCreateWindow (
        hwnd,WC_STATBAR,
        NULL,WS_VISIBLE,
        0,0,pcreate->cx-0,STATBAR_YSIZE,
        hwnd,HWND_TOP,
        ID_STATBAR,
        NULL,NULL);
    if (hwndStatbar==NULLHANDLE) printf("error! statbar\n");

// pop-up menu
    hwndPopupMenu = WinLoadMenu( hwnd, NULLHANDLE, ID_POPUP );

    WinPostMsg(hwnd,HEWM_INIT,0L,0L);

    return MRFROMLONG(FALSE);
}

MRESULT mainwmSetFocus(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (mp2)
        printf("HMLEdit:: Received Focus from %x\n",mp1);
        else
        printf("HMLEdit:: Lost Focus to %x\n",mp1);
    return 0;
}

MRESULT mainwmSize( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
//  printf("HMLEdit:: WM_SIZE\n");

    WinSetWindowPos( WinWindowFromID( hwnd, ID_HANMLE ), NULLHANDLE,
            0, (use_statbar?STATBAR_YSIZE:0),
            SHORT1FROMMP(mp2),
            SHORT2FROMMP(mp2)-(use_statbar?STATBAR_YSIZE:0)-(use_toolbar?TOOLBAR_YSIZE:0),
            SWP_SIZE | SWP_MOVE );
    if (use_toolbar)
        WinSetWindowPos( WinWindowFromID( hwnd, ID_TOOLBAR ), NULLHANDLE,
            0, SHORT2FROMMP(mp2)-TOOLBAR_YSIZE, SHORT1FROMMP(mp2) , TOOLBAR_YSIZE,
            SWP_SIZE | SWP_MOVE | SWP_SHOW);
        else
        WinSetWindowPos( WinWindowFromID( hwnd, ID_TOOLBAR ), NULLHANDLE,
            0, SHORT2FROMMP(mp2)-TOOLBAR_YSIZE, SHORT1FROMMP(mp2) , TOOLBAR_YSIZE,
            SWP_SIZE | SWP_MOVE | SWP_HIDE);
    if (use_statbar)
        WinSetWindowPos( WinWindowFromID( hwnd, ID_STATBAR ), NULLHANDLE,
            0, 0, SHORT1FROMMP(mp2), STATBAR_YSIZE,
            SWP_SIZE | SWP_MOVE | SWP_SHOW);
        else
        WinSetWindowPos( WinWindowFromID( hwnd, ID_STATBAR ), NULLHANDLE,
            0, 0, SHORT1FROMMP(mp2), STATBAR_YSIZE,
            SWP_SIZE | SWP_MOVE | SWP_HIDE);
    return MRFROMLONG(0L);
}

MRESULT mainwmChar(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
//USHORT  fsFlags = SHORT1FROMMP(mp1);
//UCHAR   ucVkey  = CHAR3FROMMP(mp2);
//UCHAR   ucChar  = CHAR1FROMMP(mp2);

//UCHAR ucScancode = CHAR4FROMMP(mp1);
//char str[100];
//  printf("HMLEdit:: WM_CHAR\n");

//    ucChar = toupper(ucChar);
    //if (fsFlags & KC_KEYUP) return 0L;
    //if ((ucChar == 'S')&&(fsFlags&KC_CTRL)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_SHIFT))
        //HESave(hwnd);
    //if ((ucChar == 'O')&&(fsFlags&KC_CTRL)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_SHIFT))
        //HEOpen(hwnd);
    //if ((ucChar == 'I')&&(fsFlags&KC_CTRL)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_SHIFT))
        //HEImport(hwnd);
    //if ((ucChar == 'F')&&(fsFlags&KC_CTRL)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_SHIFT))
        //HEFind(hwnd);
    //if ((ucChar == '/')&&(fsFlags&KC_CTRL)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_SHIFT))
        //HESelectAll(hwnd);
    //if ((ucChar == 'X')&&(fsFlags&KC_ALT)&&!(fsFlags&KC_CTRL)&&!(fsFlags&KC_SHIFT))
        //HEExit(hwnd);
    //if ((ucVkey == VK_F2)&&!(fsFlags&KC_ALT)&&!(fsFlags&KC_CTRL)&&!(fsFlags&KC_SHIFT))
        //WinSendMsg( hwnd, WM_COMMAND, MPFROMSHORT( IDM_RELOAD ), 0 );

    return MRFROMLONG( TRUE );
}

MRESULT mainwmButton2Up( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    POINTS  pts;
    ULONG   fs = PU_POSITIONONITEM | PU_NONE | PU_SELECTITEM | PU_KEYBOARD | PU_MOUSEBUTTON1;

    WinEnableMenuItem( hwndPopupMenu, IDM_COPY, markingState );
    WinEnableMenuItem( hwndPopupMenu, IDM_CUT, markingState );
    WinEnableMenuItem( hwndPopupMenu, IDM_CLEAR, markingState );

    pts.x = SHORT1FROMMP( mp1 );
    pts.y = SHORT2FROMMP( mp1 );

    WinPopupMenu( hwnd, hwnd, hwndPopupMenu, pts.x, pts.y, IDM_PASTE, fs );

    return 0L;
}


MRESULT mainwmClose(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
//  printf("HMLEdit:: WM_CLOSE\n");

    if (WinSendMsg(hwndHMLE,HMLM_QUERYCHANGED,0L,0L))
        {
        if (WinMessageBox(
                HWND_DESKTOP, hwnd,
                "Exit without saving?","Caution",
                0,MB_OKCANCEL|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_CANCEL)
            return 0L;
        }

    WinSendMsg(hwndHMLE,WM_CLOSE,0L,0L);
    WinSendMsg(hwndStatbar,WM_CLOSE,0L,0L);
    WinPostMsg(hwnd,WM_QUIT,0L,0L);
    return 0L;
}

MRESULT mainwmControl( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    switch (SHORT1FROMMP(mp1))
    {
    case ID_HANMLE:
//      printf("HMLEdit:: WM_CONTROL\n");
//      printf("notif = %x\n",SHORT2FROMMP(mp1));
        switch (SHORT2FROMMP(mp1))
        {
        case HMLN_CHANGED:
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED,MPFROMLONG(TRUE),0L);
            if (WinSendMsg(hwndToolbar,TOOLBAR_USERM_ISITEMSHOWING,MPFROMLONG(TOOLBAR_RELOAD),0))
                DisableReloadButton(hwnd);
            break;
        case HMLN_BEGINSELECTION:
            markingState = TRUE;

            if (use_smarttoolbar) ShowEditButtons(hwnd);
            break;
        case HMLN_ENDSELECTION:
            markingState = FALSE;

            if (use_smarttoolbar) HideEditButtons(hwnd);
            break;
        case HMLN_KILLFOCUS:
            break;
        case HMLN_SETFOCUS:
            break;
        case HMLN_FILEDROP:
            {
            HMLEFILEDROPNOTIF *dropFileNotif = (HMLEFILEDROPNOTIF*)mp2;

            if (WinSendMsg(hwndHMLE,HMLM_QUERYCHANGED,0L,0L))
                {
                if (WinMessageBox(
                        HWND_DESKTOP,hwnd,
                        "Document changed. Discard it?","Caution",
                        0,MB_YESNO|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_NO)
                    return 0L;
                }
            HEFileOpenWithAutoHanCode(hwnd,dropFileNotif->fullPath,TRUE);

            } break;
        case HMLN_NOTIFYCURSORPOS:
            {
                WinPostMsg(hwndStatbar,STATBAR_USERM_SETLINECOL,mp2,0L);
            } break;
        } // switch NotifiCode;
        break;
    case ID_HIA:
        switch (SHORT2FROMMP(mp1))
        {
        case HIAN_HANMODECHANGED:
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANMODE,mp2,0L);
            break;
        case HIAN_KBDTYPECHANGED:
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETKBDTYPE,mp2,0L);
            break;
        case HIAN_INSERTMODECHANGED:
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETINSERTMODE,mp2,0L);
        } // switch NotifCode
        break;
    } // switch Control

    return MRFROMLONG(0L);
}

MRESULT mainwmCommand( HWND hwnd, MPARAM mp1, MPARAM mp2 )
{
    switch (SHORT1FROMMP(mp1))
    {
    case IDM_NEW:
        HENew(hwnd);
        break;
    case IDM_OPEN:
        HEOpen(hwnd);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
        break;
    case IDM_IMPORT:
        HEImport(hwnd);
        break;
    case IDM_RELOAD:
        if (WinSendMsg(hwndHMLE,HMLM_QUERYCHANGED,0L,0L))
            {
            if (WinMessageBox(
                    HWND_DESKTOP,hwnd,
                    "Document changed. Discard it?","Caution",
                    0,MB_YESNO|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_NO)
                return 0L;
            }
        HEReloadAfterChangeHanCode(hwnd);
        break;
    case IDM_QUICKRELOAD:
        HEReloadAfterChangeHanCode(hwnd);
        break;
    case IDM_SAVE:
        HESave(hwnd);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
        break;
    case IDM_SAVEAS:
        HESaveAs(hwnd);
        break;
    case IDM_EXIT:
        HEExit(hwnd);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
//        return 0L;
        break;
    case IDM_ABOUT:
        HEAbout(hwnd);
        break;
    case IDM_OPTIONS:
        HEOptions(hwnd);
        break;
    case IDM_COPY:
        WinSendMsg(hwndHMLE,HMLM_COPY,0,0);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
        break;
    case IDM_CUT:
        WinSendMsg(hwndHMLE,HMLM_CUT,0,0);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
        break;
    case IDM_PASTE:
        WinSendMsg(hwndHMLE,HMLM_PASTE,0,0);
        break;
    case IDM_CLEAR:
        WinSendMsg(hwndHMLE,HMLM_CLEAR,0,0);
//        WinSetFocus(HWND_DESKTOP,hwndHMLE);
        break;
    case IDM_FIND:
        HEFind(hwnd);
        break;
    case IDM_SELECTALL:
        HESelectAll(hwnd);
        break;
    case IDM_SPECIALCHARS:
        HESpecialChars( hwnd );
        break;
    case IDM_HANJA:
        HEHanja( hwnd );
        break;
    } // switch Control

    if( SHORT1FROMMP( mp2 ) == CMDSRC_PUSHBUTTON )
        WinSetFocus(HWND_DESKTOP,hwndHMLE);

    WinPostMsg( hwndHMLE, HMLM_REFRESH, 0, 0 );

    return MRFROMLONG(0L);
}

ULONG HEExit(HWND hwnd)
{
    WinSendMsg(hwnd,WM_CLOSE,0L,0L);
    return 0L;
}

ULONG HENew(HWND hwnd)
{

    if (WinSendMsg(hwndHMLE,HMLM_QUERYCHANGED,0L,0L))
        {
        if (WinMessageBox(
                HWND_DESKTOP,hwnd,
                "Document changed. Discard it?","Caution",
                0,MB_YESNO|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_NO)
            return 0L;
        }

    WinSendMsg(hwndHMLE,HMLM_NEWDOC,0L,0L);
    {
    szFullPath[0] = 0;
    HESetTitlebarText(hwnd);
    WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED,0L,0L);
    WinPostMsg(hwndHMLE,HMLM_SETCHANGED,0,0);
    DisableReloadButton(hwnd);
    }

    return 0L;
}

ULONG HEReloadAfterChangeHanCode(HWND hwnd)
{
ULONG HanType   = (ULONG)  WinSendMsg(hwndHMLE,HMLM_QUERYHANTYPE,0L,0L);

    if (*szFullPath=='\0') return 0;

    if (HanType == HMLE_HAN_SY)
    {
        WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
        WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
    } else {
        WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
        WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
    }

    HEFileOpen(hwnd,szFullPath,FALSE);
    DisableReloadButton(hwnd);

    return 0L;
}

ULONG HEOpen(HWND hwnd)
{
FILEDLG FDialog;
char    szTitle[100];

    if (WinSendMsg(hwndHMLE,HMLM_QUERYCHANGED,0L,0L))
        {
        if (WinMessageBox(
                HWND_DESKTOP,hwnd,
                "Document changed. Discard it?","Caution",
                0,MB_YESNO|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_NO)
            return 0L;
        }
    strcpy(szTitle,"Open");

    strcpy(FDialog.szFullFile,"*.*");

    FDialog.cbSize = sizeof(FILEDLG);
    FDialog.pszTitle = szTitle;
    FDialog.pszOKButton = szTitle;
    FDialog.ulUser = 0;
    FDialog.fl = FDS_CENTER | FDS_OPEN_DIALOG; //| FDS_ENABLEFILELB;
    FDialog.pfnDlgProc = (PFNWP) NULL;
    FDialog.lReturn = 0;
    FDialog.lSRC = 0;
    FDialog.hMod = (HMODULE) 0;
    FDialog.usDlgId = IDD_FILEOPEN;
    FDialog.x   = 0;
    FDialog.y   = 0;
    FDialog.pszIType    = NULL;
    FDialog.papszITypeList  = NULL;
    FDialog.pszIDrive   = NULL;
    FDialog.papszIDriveList = NULL;
    FDialog.sEAType         = 0;
    FDialog.papszFQFilename = NULL;
    FDialog.ulFQFCount  = 0;

    if (!WinFileDlg(HWND_DESKTOP,hwnd,(PFILEDLG)&FDialog))
        return 1;
    if (FDialog.lReturn != DID_OK)
        return 1;

    HEFileOpenWithAutoHanCode(hwnd,FDialog.szFullFile,TRUE);

    return 0L;
}

ULONG HEImport( HWND hwnd )
{
FILEDLG FDialog;
char    szTitle[100];
char    *buf;
ULONG   size;
ULONG   hanType = (ULONG)WinSendMsg(hwndHMLE,HMLM_QUERYHANTYPE,0L,0L);

    if( WinSendMsg( hwndHMLE, HMLM_QUERYREADONLY, 0, 0 ))
        return 0L;

    strcpy(szTitle,"Import");

    strcpy(FDialog.szFullFile,"*.*");

    FDialog.cbSize = sizeof(FILEDLG);
    FDialog.pszTitle = szTitle;
    FDialog.pszOKButton = szTitle;
    FDialog.ulUser = 0;
    FDialog.fl = FDS_CENTER | FDS_OPEN_DIALOG; //| FDS_ENABLEFILELB;
    FDialog.pfnDlgProc = (PFNWP) NULL;
    FDialog.lReturn = 0;
    FDialog.lSRC = 0;
    FDialog.hMod = (HMODULE) 0;
    FDialog.usDlgId = IDD_FILEOPEN;
    FDialog.x   = 0;
    FDialog.y   = 0;
    FDialog.pszIType    = NULL;
    FDialog.papszITypeList  = NULL;
    FDialog.pszIDrive   = NULL;
    FDialog.papszIDriveList = NULL;
    FDialog.sEAType         = 0;
    FDialog.papszFQFilename = NULL;
    FDialog.ulFQFCount  = 0;

    if (!WinFileDlg(HWND_DESKTOP,hwnd,(PFILEDLG)&FDialog))
        return 1;
    if (FDialog.lReturn != DID_OK)
        return 1;

    size = fileRead2Buf( FDialog.szFullFile, &buf );
    if( buf == NULL )
        return 1;

    WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG( queryHanTypeBuf( buf )),0L);
    WinSendMsg(hwndHMLE,HMLM_SETIMPORTEXPORT,buf,MPFROMLONG(size+1));
    WinSendMsg(hwndHMLE,HMLM_IMPORT,0,0);
    WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG( hanType ),0L);

    WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED, MPFROMLONG( TRUE ),0L);
    WinPostMsg(hwndHMLE,HMLM_SETCHANGED, MPFROMLONG( TRUE ),0);
    WinPostMsg(hwndHMLE,HMLM_REFRESH,0,0);

    free( buf );

    return 0L;
}

ULONG HESave(HWND hwnd)
{
//  printf("HESave : %s\n",szFullPath);
    if (( szFullPath[0] == 0 ) || markingState )
        {
        HESaveAs(hwnd);
        } else {
        if (WinSendMsg(hwndHMLE,HMLM_QUERYREADONLY,0,0)) return 0;
        if (!HEFileSave(hwnd,szFullPath))
            {
            HESetTitlebarText(hwnd);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED,0L,0L);
            WinPostMsg(hwndHMLE,HMLM_SETCHANGED,0,0);
            DisableReloadButton(hwnd);
            } else return 1;
        }
    return 0L;
}

ULONG HESaveAs(HWND hwnd)
{
FILEDLG FDialog;
FILESTATUS3 fst3;
APIRET rc;
char    szTitle[1000];

    strcpy(szTitle,"Save as");

    if (szFullPath[0] == 0)
        strcpy(FDialog.szFullFile,"noname.txt");
        else
        strcpy(FDialog.szFullFile,szFullPath);
    FDialog.cbSize = sizeof(FILEDLG);
    FDialog.pszTitle = szTitle;
    FDialog.pszOKButton = szTitle;
    FDialog.ulUser = 0;
    FDialog.fl = FDS_CENTER | FDS_SAVEAS_DIALOG | FDS_ENABLEFILELB;
    FDialog.pfnDlgProc = (PFNWP) NULL;
    FDialog.lReturn = 0;
    FDialog.lSRC = 0;
    FDialog.hMod = (HMODULE) 0;
    FDialog.usDlgId = IDD_FILESAVEAS;
    FDialog.x   = 0;
    FDialog.y   = 0;
    FDialog.pszIType    = NULL;
    FDialog.papszITypeList  = NULL;
    FDialog.pszIDrive   = NULL;
    FDialog.papszIDriveList = NULL;
    FDialog.sEAType         = 0;
    FDialog.papszFQFilename = NULL;
    FDialog.ulFQFCount  = 0;

    if (!WinFileDlg(HWND_DESKTOP,hwnd,(PFILEDLG)&FDialog))
        return 1;
    if (FDialog.lReturn != DID_OK)
        return 1;

    rc = DosQueryPathInfo(FDialog.szFullFile,1,&fst3,sizeof(FILESTATUS3));
    if (!((rc==0) || (rc==3) || (rc==2)))
        {
        printf("DosQueryPathInfo rc = %d\t\t0x%x\n",rc,rc);
        printf("NOT SAVED.\n");
        return 1;
        }
    if (rc==0)
        {
        if (WinMessageBox(HWND_DESKTOP,hwnd,
                "File exists. Overwrite?","Caution",
                0,MB_YESNO|MB_DEFBUTTON2|MB_WARNING|MB_MOVEABLE)==MBID_NO)
            return 0L;
        }

    strcpy(szFullPath,FDialog.szFullFile);
    if (!HEFileSave(hwnd,szFullPath))
        {
        HESetTitlebarText(hwnd);
        WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED,0L,0L);
        WinPostMsg(hwndHMLE,HMLM_SETCHANGED,0,0);
        DisableReloadButton(hwnd);
        } else return 1L;
    return 0L;
}

ULONG HEAbout(HWND hwnd)
{
ULONG ulrc;

    ulrc = WinDlgBox(HWND_DESKTOP,hwnd,&WinDefDlgProc,NULLHANDLE,IDD_ABOUT,NULL);
    if (ulrc == DID_ERROR)
        {
        printf("DID_ERROR: %d,%8x\n",ulrc,WinGetLastError(WinQueryAnchorBlock(hwnd)));
        }
    return 0L;
}

ULONG HEOptions(HWND hwnd)
{
HWND hwndDlg;
ULONG TextFormat = (ULONG) WinSendMsg(hwndHMLE,HMLM_QUERYTEXTFORMAT,0L,0L);
ULONG HanType   = (ULONG)  WinSendMsg(hwndHMLE,HMLM_QUERYHANTYPE,0L,0L);
ULONG ulrc = ( ULONG ) WinSendMsg( hwndHMLE, HMLM_QUERYWRAP, 0, 0 );
//int old_eoltype = TextFormat;
//int old_hantype = HanType;
BOOL wordWrap = SHORT1FROMMR( ulrc );
int  wordWrapSize = SHORT2FROMMR( ulrc );
CHAR size[ 10 ];

    hwndDlg = WinLoadDlg(HWND_DESKTOP,hwnd,&WinDefDlgProc,NULLHANDLE,IDD_OPTIONS,NULL);

    if (TextFormat==HMLE_EOL_CRLF)
        WinSendMsg(WinWindowFromID(hwndDlg,IDB_TF_CRLF),BM_SETCHECK,MPFROMLONG(1L),0L);
        else
        WinSendMsg(WinWindowFromID(hwndDlg,IDB_TF_LF),BM_SETCHECK,MPFROMLONG(1L),0L);
    if (HanType==HMLE_HAN_KS)
        WinSendMsg(WinWindowFromID(hwndDlg,IDB_HT_KS),BM_SETCHECK,MPFROMLONG(1L),0L);
        else
        WinSendMsg(WinWindowFromID(hwndDlg,IDB_HT_SY),BM_SETCHECK,MPFROMLONG(1L),0L);

    WinSendMsg( WinWindowFromID( hwndDlg, IDB_WORDWRAP ), BM_SETCHECK,
                MPFROMLONG( SHORT1FROMMR( wordWrap )), 0 );

    _itoa( wordWrapSize, size, 10 );
    WinSetWindowText( WinWindowFromID( hwndDlg, IDEF_WORDWRAP_SIZE ), size );

    //WinSetFocus(HWND_DESKTOP,hwndDlg);

    ulrc = WinProcessDlg(hwndDlg);

    if (ulrc == DID_OK)
        {
        TextFormat  = (ULONG) WinSendMsg(WinWindowFromID(hwndDlg,IDB_TF_CRLF),BM_QUERYCHECKINDEX,0L,0L);
        HanType     = (ULONG) WinSendMsg(WinWindowFromID(hwndDlg,IDB_HT_KS),BM_QUERYCHECKINDEX,0L,0L);
        if (TextFormat == 1)
            {
            WinSendMsg(hwndHMLE,HMLM_SETTEXTFORMAT,MPFROMLONG(HMLE_EOL_CRLF),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETEOLTYPE,MPFROMLONG(HMLE_EOL_CRLF),0L);
            } else {
            WinSendMsg(hwndHMLE,HMLM_SETTEXTFORMAT,MPFROMLONG(HMLE_EOL_LF),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETEOLTYPE,MPFROMLONG(HMLE_EOL_LF),0L);
            }
        if (HanType == 1)
            {
            WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
            } else {
            WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
            }

        wordWrap = ( BOOL )WinSendDlgItemMsg( hwndDlg, IDB_WORDWRAP, BM_QUERYCHECK, 0, 0 );
        WinQueryWindowText( WinWindowFromID( hwndDlg, IDEF_WORDWRAP_SIZE ), sizeof( size ), size );
        wordWrapSize = atoi( size );
        if(( wordWrapSize != 0 ) && ( wordWrapSize < MIN_WORDWRAP_SIZE ))
            wordWrapSize = MIN_WORDWRAP_SIZE;

        WinSendMsg( hwndHMLE, HMLM_SETWRAP, MPFROMLONG( wordWrap ), MPFROMSHORT( wordWrapSize ));
        }
    WinDestroyWindow(hwndDlg);
    return 0L;
}

ULONG HEFind(HWND hwnd)
{
static char findStr[ 256 ] = { 0, };
static char replaceStr[ 256 ] = { 0, };

ULONG ulrc;
HWND hwndDlg;
HWND hwndFind;
HWND hwndReplace;
//char str[255];
//int hanmode,kbdtype;

    hwndDlg = WinLoadDlg(HWND_DESKTOP,hwnd,&HEFindDlgProc,NULLHANDLE,IDD_FIND,NULL);
    hwndFind = WinWindowFromID( hwndDlg, IDHEF_FIND );
    hwndReplace = WinWindowFromID( hwndDlg, IDHEF_REPLACE );

    WinSetWindowText( hwndFind, findStr );
//    WinSendMsg( hwndFind, HEM_SETSEL, MPFROM2SHORT( 0, strlen( findStr )), 0 );
//    WinSendMsg( hwndFind, HEM_REFRESH, 0, 0 );
    WinSetWindowText( hwndReplace, replaceStr );
//    WinSendMsg( hwndReplace, HEM_SETSEL,
//                MPFROM2SHORT( strlen( replaceStr ), strlen( replaceStr )), 0 );
//    WinSendMsg( hwndReplace, HEM_REFRESH, 0, 0 );

    WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwndDlg, DID_OK ));

    ulrc = WinProcessDlg(hwndDlg);

//  if (ulrc == DID_OK)
//    {
//    }

    WinQueryWindowText( hwndFind, sizeof( findStr ), findStr );
    WinQueryWindowText( hwndReplace, sizeof( replaceStr ), replaceStr );

    WinDestroyWindow(hwndDlg);

//  hanmode = (int)WinSendMsg(hwndHMLE,HMLM_QUERYHANINPUTMODE,0,0);
//  kbdtype = (int)WinSendMsg(hwndHMLE,HMLM_QUERYKBDTYPE,0,0);

//  WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANMODE,MPFROMLONG(hanmode),0L);
//  WinPostMsg(hwndStatbar,STATBAR_USERM_SETKBDTYPE,MPFROMLONG(kbdtype),0L);

    WinSetFocus(HWND_DESKTOP,hwndHMLE);
    return 0L;
}

ULONG HESelectAll(HWND hwnd)
{
int size = (int)WinSendMsg(hwndHMLE,HMLM_QUERYTEXTLENGTH,0,0);

    WinPostMsg(hwndHMLE,HMLM_SETSEL,0,MPFROMLONG(size));

    return 0L;
}

ULONG HESpecialChars( HWND hwnd )
{
    WinSendMsg( hwndHIA, WM_CHAR, MPFROM2SHORT( KC_VIRTUALKEY, 0 ), MPFROM2SHORT( 0, VK_F4 ));

    return 0;
}

ULONG HEHanja( HWND hwnd )
{
    WinSendMsg( hwndHIA, WM_CHAR, MPFROM2SHORT( KC_VIRTUALKEY, 0 ), MPFROM2SHORT( 0, VK_F9 ));

    return 0;
}

ULONG HESetTitlebarText(HWND hwnd)
{
HWND hwndOwner = WinQueryWindow(hwnd,QW_PARENT);
char str[CCHMAXPATH];

    if (readonly)
        {
        if (*(szFullPath))
            sprintf(str,"(Read only) %s",szFullPath);
            else
            sprintf(str,"(Read only) (UNTITLED)");
        } else {
        if (*(szFullPath))
            sprintf(str,"%s",szFullPath);
            else
            sprintf(str,"(UNTITLED)");
        }

// TODO: update switchlist entry

    WinSetWindowText(WinWindowFromID(hwndOwner,FID_TITLEBAR),str);
    return 0L;
}

int HEFileOpenWithAutoHanCode(HWND hwnd,char *filename,int enableReload)
{
    ULONG HanType   = (ULONG)  WinSendMsg(hwndHMLE,HMLM_QUERYHANTYPE,0L,0L);
    int ret;

    _fullpath( szFullPath, filename, sizeof( szFullPath ));
    ret = HEFileOpen( hwnd, szFullPath, enableReload );

    if(( ret != -1 ) && ( ret != ( int )HanType ))
    {
/*
        if( ret == HMLE_HAN_SY )
        {
            WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_SY),0L);
        } else {
            WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
            WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(HMLE_HAN_KS),0L);
        }
*/
        WinSendMsg(hwndHMLE,HMLM_SETHANTYPE,MPFROMLONG(ret),0L);
        WinPostMsg(hwndStatbar,STATBAR_USERM_SETHANTYPE,MPFROMLONG(ret),0L);

        ret = HEFileOpen( hwnd, szFullPath, enableReload );
    }

    return ret;
}

int HEFileOpen(HWND hwnd,char *filename,int enableReload)
{
char *buf = NULL;
ULONG size;
int ret = 0;

    WinSendMsg(hwndHMLE,HMLM_NEWDOC,0L,0L);

    size = fileRead2Buf( filename, &buf );

    if( buf != NULL )
    {
        BOOL readOnly = ( BOOL )WinSendMsg( hwndHMLE, HMLM_QUERYREADONLY, 0, 0 );

        WinSendMsg( hwndHMLE, HMLM_SETREADONLY, MPFROMLONG( FALSE ), 0 );

        WinSendMsg(hwndHMLE,HMLM_SETIMPORTEXPORT,buf,MPFROMLONG(size+1));
        WinSendMsg(hwndHMLE,HMLM_IMPORT,0,0);
        WinSendMsg(hwndHMLE,HMLM_SETFIRSTCHAR,MPFROMLONG(0),0);

        WinSendMsg( hwndHMLE, HMLM_SETREADONLY, MPFROMLONG( readOnly ), 0 );
/*
        _fullpath( szFullPath,filename, sizeof( szFullPath ));
        extractDirname();
        _chdir2(szDir);
*/
        if (enableReload) EnableReloadButton(hwnd);
        ret = queryHanTypeBuf( buf );
        free( buf );
    }
    else
    {
//        szFullPath[0]=0;
        ret = -1;
    }

    _fullpath( szFullPath,filename, sizeof( szFullPath ));
    extractDirname();
    _chdir2(szDir);

    HESetTitlebarText(hwnd);
    WinPostMsg(hwndStatbar,STATBAR_USERM_SETMODIFIED,0L,0L);
    WinPostMsg(hwndHMLE,HMLM_SETCHANGED,0,0);
    WinPostMsg(hwndHMLE,HMLM_REFRESH,0,0);

    return ret;
}

int HEFileSave(HWND hwnd,char *filename)
{
FILE *f;
char *buf;
int bufsize;

    if( markingState )
    {
        bufsize = ( int )WinSendMsg( hwndHMLE, HMLM_QUERYSELTEXT, 0, 0 );
        if( bufsize == -1 )
            return -1;

        buf = malloc( bufsize + 1 );
        if( buf == NULL )
            return -1;

        WinSendMsg( hwndHMLE, HMLM_QUERYSELTEXT, buf, 0 );
    }
    else
    {
        bufsize = (int)WinSendMsg(hwndHMLE,HMLM_QUERYFORMATTEXTLENGTH,0,0);
//      printf("bufsize = %d\n",bufsize);
        if (bufsize==-1) return -1;
        buf = malloc(bufsize+1);
        if (buf==NULL) return -1;
        WinSendMsg(hwndHMLE,HMLM_SETIMPORTEXPORT,buf,MPFROMLONG(bufsize+1));
        WinSendMsg(hwndHMLE,HMLM_EXPORT,0,0);
    }

    f = fopen(filename,"wb");
    if (f==NULL)
        {
        free(buf);
        return -1;
        }

    fwrite(buf,bufsize,1,f);
    fclose(f);

    free(buf);

    return 0;
}

ULONG fileRead2Buf( char *filename, char **buf )
{
    FILE *f;
    struct stat fileStat;

    *buf = NULL;
    f = fopen(filename,"rb");
    if (f==NULL)
        return 0;

    if (stat(filename,&fileStat)==-1)
        return 0;

    *buf = malloc(fileStat.st_size+1);
    if (*buf==NULL)
        return 0;

    fread(*buf,fileStat.st_size,1,f);
    (*buf)[fileStat.st_size]=0;

    fclose( f );

    return fileStat.st_size;
}

ULONG queryHanTypeBuf( char *buf )
{
    THCTYPE hct;

    if((hct = queryHCTStr( buf )) == UNKNOWN )
        hct = WANSUNG;

    return( hct == WANSUNG ? HMLE_HAN_KS : HMLE_HAN_SY );
}

void extractDirname(void)
{
char* lastslash;
int i;

    lastslash = strrchr(szFullPath,'\\');
    if (lastslash==NULL)
        {
        strcpy(szDir,szFullPath);
        return;
        }
    for (i=0;szFullPath+i<lastslash;i++)
        szDir[i] = szFullPath[i];
    szDir[i] = '\0';
}

void EnableReloadButton(HWND hwnd)
{
HAB hab = WinQueryAnchorBlock(hwnd);

    if (!use_smarttoolbar) return;
    if (*szFullPath=='\0') return;

    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_RELOAD),MPFROMLONG(TRUE));
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_UPDATE,0,0);

    WinStartTimer(hab,hwnd,ID_TIMER,10000);
}

void DisableReloadButton(HWND hwnd)
{
HAB hab = WinQueryAnchorBlock(hwnd);

    if (!use_smarttoolbar) return;

    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_RELOAD),MPFROMLONG(FALSE));
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_UPDATE,0,0);

    WinStopTimer(hab,hwnd,ID_TIMER);
}

MRESULT mainwmTimer(HWND hwnd,MPARAM mp1,MPARAM mp2)
{
    if (SHORT1FROMMP(mp1)==ID_TIMER)
        DisableReloadButton(hwnd);
    return 0L;
}

void HideEditButtons(HWND hwnd)
{
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_COPY),FALSE);
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CUT),FALSE);
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_PASTE),FALSE);
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CLEAR),FALSE);
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_UPDATE,0,0);
}

void ShowEditButtons(HWND hwnd)
{
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_COPY),MPFROMLONG(TRUE));
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CUT),MPFROMLONG(TRUE));
//  WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_PASTE),MPFROMLONG(TRUE));
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_SHOWITEM,MPFROMLONG(TOOLBAR_CLEAR),MPFROMLONG(TRUE));
    WinPostMsg(hwndToolbar,TOOLBAR_USERM_UPDATE,0,0);
}

