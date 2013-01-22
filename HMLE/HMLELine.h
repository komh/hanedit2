#ifndef __HMLELINE_H__
#define __HMLELINE_H__

typedef struct tagHMLELine HMLELine;
struct tagHMLELine {
    UCHAR*      str;
    ULONG       allocsize;
    BOOL        wordWrapped;
    HMLELine*   prevLine;
    HMLELine*   nextLine;
};

HMLELine* HMLECreateLine(char* str);
void HMLEDestroyLine(HMLELine* this);
int HMLELineLinkAfter(HMLELine* this,HMLELine* prevLine);
int HMLELineQueryLen(HMLELine* this);
char* HMLELineQueryStr(HMLELine* this,int stx);
int HMLELineIsPacked(HMLELine* this);
int HMLELinePack(HMLELine* this);
int HMLELineUnpack(HMLELine* this,int newsize);
HMLELine* HMLELineQueryLine(HMLELine* this,int step);
HMLELine* HMLELineQueryLine_NullSafe(HMLELine* this,int step);
int HMLELineQueryLineNumber(HMLELine* this);
int HMLELineFindFirstCharFromStx(HMLELine *this,char c,int stx);
HMLELine* HMLELineSplit(HMLELine *this,int stx);
int HMLELineCombineWithNextLine(HMLELine *this);
int HMLELineWordWrap( HMLELine *this, int size, BOOL wordProtect, int tabSize );
int HMLELineQueryWordWrapInfo( HMLELine *this, int size, BOOL wordProtect, int tabSize, int *stx );
int HMLELineInsertPSLF(HMLELine *this,int stx,PSLF str);
int HMLELineInsertPSLF_limit(HMLELine *this,int stx,PSLF str,int limit);
int HMLELineInsertHch(HMLELine *this,int stx,HANCHAR hch);
HANCHAR HMLELineDeleteHch(HMLELine *this,int stx);
int HMLELineDeleteFrom(HMLELine *this,int stx); // in-bound
int HMLELineDeleteTo(HMLELine *this,int stx);   // ex-bound
int HMLELineDeleteFromTo(HMLELine *this,int fromStx,int toStx);
int HMLELineNextWord(HMLELine* this,int stx,const char* separators);
int HMLELinePrevWord(HMLELine* this,int stx,const char* separators);

int HMLELineSearchStr(HMLELine* this,int stx,const char* str,int caseSens);

#endif // __HMLELINE_H__
