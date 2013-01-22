#ifndef __HMLEDOC_H__
#define __HMLEDOC_H__

#define HMLEDOC_MARKING_NONE    0
#define HMLEDOC_MARKING_BEGIN   1

#define HMLEDOC_ERROR_NOERROR               0
#define HMLEDOC_ERROR_NULLINSTANCE          1
#define HMLEDOC_ERROR_INVALIDARG            2
#define HMLEDOC_ERROR_INSERTIONTRUNCATED    3

typedef struct tagHMLEDoc HMLEDoc;
struct tagHMLEDoc {
    HMLELine*   beginLine;
    HMLELine*   endLine;
    HMLELine*   curLine;
    ULONG       curStx;
    ULONG       savedStx;
    HMLELine*   anchorLine;
    ULONG       anchorStx;
    ULONG       markingState;
    ULONG       editBufSize;
    USHORT      hchComposing;
    USHORT      tabsize;
    UCHAR*      separators;
    ULONG       changed;
//  ULONG       readonly;
//  ULONG       textformat;
//  ULONG       TGtype;
    int         errno;
};

HMLEDoc* HMLECreateDoc(int cbEditBuf);
void HMLEDestroyDoc(HMLEDoc* this);
void HMLEDocSetSeparators(HMLEDoc* this,const char* separators);
int HMLEDocInsertTextThunk(HMLEDoc* this,HMLETextThunk *textThunk);
HMLETextThunk* HMLEDocCreateTextThunk(HMLEDoc* this,HMLEIpt *ipt1,HMLEIpt *ipt2);
int HMLEDocDelete(HMLEDoc *this,HMLEIpt *beginIpt,HMLEIpt *endIpt);
int HMLEDocQuerySize(HMLEDoc* this,HMLEIpt *ipt1,HMLEIpt *ipt2);

HMLELine* HMLEDocQueryLine(HMLEDoc* this,int ln);
HMLELine *HMLEDocQueryEndLine(HMLEDoc *this);
int HMLEDocQueryNumberOfLines(HMLEDoc *this);
int HMLEDocQueryMaxCols(HMLEDoc *this);
int HMLEDocPackCurLine(HMLEDoc* this);
int HMLEDocUnpackCurLine(HMLEDoc* this);
HMLELine* HMLEDocMoveCurLineTo(HMLEDoc* this,int idx);
int HMLEDocMoveCurLine(HMLEDoc* this,int step);
int HMLEDocMoveCurLineNext(HMLEDoc* this);
int HMLEDocMoveCurLinePrev(HMLEDoc* this);
int HMLEDocMoveCurStxTo(HMLEDoc* this,int stx);
int HMLEDocMoveCurStx(HMLEDoc *this,int step);
int HMLEDocMoveCurStxLeft(HMLEDoc *this,int step);
int HMLEDocMoveCurStxRight(HMLEDoc *this,int step);
int HMLEDocMoveNextWord(HMLEDoc *this);
int HMLEDocMovePrevWord(HMLEDoc *this);

HMLELine* HMLEDocSplitCurLine(HMLEDoc* this);
int HMLEDocCombineLines(HMLEDoc *this);

int HMLEDocQueryOfs(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocQueryIpt(HMLEDoc *this,HMLEIpt *ipt,int ofs);
int HMLEDocQueryCurIpt(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocQueryAnchorIpt(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocQuerySelectionBeginIpt(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocQuerySelectionEndIpt(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocSetCurIpt(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocInsertHch(HMLEDoc *this,HANCHAR hch);
HANCHAR HMLEDocDeleteHch(HMLEDoc *this);
int HMLEDocStreamOut(HMLEDoc* this,FILE *stream);

int HMLEDocBeginSelectionHere(HMLEDoc *this,HMLEIpt *ipt);
int HMLEDocEndSelection(HMLEDoc *this);
int HMLEDocQueryMarkingState(HMLEDoc *this);
HMLETextThunk* HMLEDocGetSelection(HMLEDoc *this);
int HMLEDocDeleteSelection(HMLEDoc *this);

int HMLEDocQueryTabsizeCol(HMLEDoc *this,int col);
int HMLEDocQueryTabspaceCol(HMLEDoc *this,int col);
int HMLEDocQueryTabsizeStx(HMLEDoc *this,HMLELine *line,int stx);
int HMLEDocQueryTabspaceStx(HMLEDoc *this,HMLELine *line,int stx);
int HMLEDocStx2Col(HMLEDoc *this,HMLELine *line,int stx);
int HMLEDocCol2Stx(HMLEDoc *this,HMLELine *line,int col);
int HMLEDocFormatLine(HMLEDoc *this,HMLELine *line,char *str);
int HMLEDocFormatLine2(HMLEDoc *this,HMLELine *line,char *str,int fixedLen);

/*int HMLEDocFindFirst(HMLEDoc *this,const char* text,int caseSens);
int HMLEDocFindNext(HMLEDoc *this,const char* text,int caseSens);
*/
int HMLEDocFindString(HMLEDoc *this,const char* findStr,
                        HMLEIpt *beginIpt,HMLEIpt *endIpt,HMLEIpt *foundIpt,
                        int caseSens);


void HMLEDocCheckPacking(HMLEDoc *this);
int HMLEDocReport(HMLEDoc* this,FILE *stream);
void HMLEDocDump(HMLEDoc *this,FILE *stream,int options);
int HMLEDocColFromStx(HMLEDoc *this, int stx);

#endif // __HMLEDOC_H__
