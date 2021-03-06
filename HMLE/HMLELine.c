#include <string.h>
#include "HMLE_internal.h"

static int queryTabspaceCol( HMLELine *this, int col, int tabSize );
static int queryLenWithTabSize( HMLELine *this, int tabSize );
static int col2stx( HMLELine *this, int col, int tabSize );

HMLELine* HMLECreateLine(char* str)
{
HMLELine *line;
int newsize;
char* newstr;
int len;

    if (str==NULL) return NULL;
//  printf("here?\n");
    line = malloc(sizeof(HMLELine));
    if (line==NULL) return NULL;

    len = PSLFstrlen(str);
//  printf("newline len = %d\n",len);
    newsize = len+1;
    newstr = malloc(newsize);
    if (newstr == NULL)
        {
        free(line);
        return NULL;
        } else {
        memcpy(newstr,str,len);
        newstr[len]='\0';
//      PSLFstrcpy(newstr,str);
//      PSLFtoPSZ(newstr);
        }

    line->allocsize = newsize;
    line->str = newstr;
    line->wordWrapped = FALSE;
    line->prevLine = NULL;
    line->nextLine = NULL;

    return line;
}

void HMLEDestroyLine(HMLELine* this)
{

    if (this==NULL) return;

    if (this->prevLine != NULL)
        this->prevLine->nextLine = this->nextLine;
    if (this->nextLine != NULL)
        this->nextLine->prevLine = this->prevLine;

    free(this->str);
    free(this);
}

int HMLELineLinkAfter(HMLELine* this,HMLELine* prevLine)
{
    if (this==NULL) return -1;
    if (prevLine==NULL) return -1;
    if (this->prevLine!=NULL) return -1;
    if (this->nextLine!=NULL) return -1;

    this->prevLine = prevLine;
    if (prevLine->nextLine!=NULL)
        {
        this->nextLine = prevLine->nextLine;
        this->nextLine->prevLine = this;
        }
    prevLine->nextLine = this;
    return 0;
}

int HMLELineQueryLen(HMLELine* this)
{
    if (this==NULL) return -1;
    return strlen(this->str);
}

char* HMLELineQueryStr(HMLELine* this,int stx)
{
    if (this==NULL) return NULL;
    if (HMLELineQueryLen(this)<stx) return NULL;
    return (this->str)+stx;
}

int HMLELineIsPacked(HMLELine* this)
{
    if (this==NULL) return -1;
    return (this->allocsize==strlen(this->str)+1);
}

int HMLELinePack(HMLELine* this)
{
int newsize;
char *newstr;

    if (this==NULL) return -1;

    if (HMLELineIsPacked(this)) return 0;

    newsize = strlen(this->str)+1;
    newstr = malloc(newsize);
    if (newstr==NULL) return -1;
    memcpy(newstr,this->str,newsize);
//  strcpy(newstr,this->str);
    free(this->str);
    this->str = newstr;
    this->allocsize = newsize;

    return 0;
}

int HMLELineUnpack(HMLELine* this,int newsize)
{
char *newstr;

    if (this==NULL) return -1;
    if (newsize < this->allocsize) return -1;
    if (newsize == this->allocsize) return 0;

    newstr = malloc(newsize);
    if (newstr==NULL) return -1;
    strcpy(newstr,this->str);
    free(this->str);

    this->allocsize = newsize;
    this->str = newstr;
    return 0;
}

HMLELine* HMLELineQueryLine(HMLELine* this,int step)
{
HMLELine* line;

    if (this==NULL) return NULL;
    if (step == 0) return this;

    line = this;
    if (step>0)
        {
        while (step > 0)
            {
            if (line->nextLine)
                line = line->nextLine;
                else return NULL;
            step--;
            }
        } else {
        while (step < 0)
            {
            if (line->prevLine)
                line = line->prevLine;
                else return NULL;
            step++;
            }
        }
    return line;
}

HMLELine* HMLELineQueryLine_NullSafe(HMLELine* this,int step)
{
HMLELine* line;

    if (this==NULL) return NULL;
    if (step == 0) return this;

    line = this;
    if (step>0)
        {
        while (step > 0)
            {
            if (line->nextLine)
                line = line->nextLine;
                else return line;
            step--;
            }
        } else {
        while (step < 0)
            {
            if (line->prevLine)
                line = line->prevLine;
                else return line;
            step++;
            }
        }
    return line;
}

int HMLELineQueryLineNumber(HMLELine* this)
{
HMLELine *line;
int lineNumber = 0;

    if (this==NULL) return -1;

    line = this->prevLine;

    while (line)
        {
        lineNumber++;
        line = line->prevLine;
        }

    return lineNumber;
}

PSZ HMLELineCopyPSZFromStx(HMLELine *this,PSZ dst,int stx) // stx : inbounds
{
    if (this==NULL) return NULL;
    if (dst==NULL) return NULL;
    if (stx>PSLFstrlen(this->str)) return NULL;

    PSLFstrcpy(dst,this->str+stx);
    return PSLFtoPSZ(dst);
}

PSZ HMLELineCopyPSZToStx(HMLELine *this,PSZ dst,int stx) // stx : ex-bounds
{
    if (this==NULL) return NULL;
    if (dst==NULL) return NULL;
    if (stx>PSLFstrlen(this->str)) return NULL;

    PSLFstrncpy(dst,this->str,stx);
    return PSLFtoPSZ(dst);
}

int HMLELineFindFirstCharFromStx(HMLELine *this,char c,int stx)
{
int idx = stx;
int len;

    if (this==NULL) return -1;
    len = HMLELineQueryLen(this);

    while (idx<len)
        {
        if (c == this->str[idx])
            return idx;
        idx++;
        }
    return -1;  // not found
}

HMLELine* HMLELineSplit(HMLELine *this,int stx)
{
HMLELine* newLine = NULL;

    if (this==NULL) return NULL;
    if (stx>HMLELineQueryLen(this)) return NULL;

//  printf("HMLELineSplit:here\n");

    newLine = HMLECreateLine((this->str)+stx);
//  if (newLine==NULL) printf("error!\n");
    newLine->wordWrapped = this->wordWrapped;

    this->str[stx] = '\0';
    this->wordWrapped = FALSE;
    HMLELinePack(this);
    HMLELineLinkAfter(newLine,this);

    return newLine;
}

int HMLELineCombineWithNextLine(HMLELine *this)
{
int newsize;
char *newstr;

    if (this==NULL) return -1;
    if (this->nextLine==NULL) return -1;

    newsize = HMLELineQueryLen(this)+HMLELineQueryLen(this->nextLine)+1;
    newstr = malloc(newsize);
    strcpy(newstr,this->str);
    strcat(newstr,HMLELineQueryStr(this->nextLine,0));
    free(this->str);
    this->str = newstr;
    this->allocsize = newsize;
    this->wordWrapped = this->nextLine->wordWrapped;
    HMLEDestroyLine(this->nextLine);
    return 0;
}

int HMLELineWordWrap( HMLELine *this, int size, BOOL wordProtect, int tabSize )
{
    int alignStx;
    HMLELine *newLine;

    if( this == NULL ) return -1;
    if( size < 0 ) return -1;

    while( this->wordWrapped )
        HMLELineCombineWithNextLine( this );

    if( size == 0 ) return 0;

    while( queryLenWithTabSize( this, tabSize ) > size )
    {
        alignStx = col2stx( this, size, tabSize );

        if( wordProtect )
        {
            char *str = HMLELineQueryStr( this, 0 );
            char *p;

#ifdef DEBUG
            printf("size %d, alignStx %d, str %p, len %d\n", size, alignStx, str + alignStx, HMLELineQueryLen( this ));
#endif
            for( p = str + alignStx - 1; p > str; p-- )
            {
                if(( *p == ' ' ) || ( *p == '\t' ))
                    break;
            }

            if(( *p != ' ' ) && ( *p != '\t' ))
            {
                for( p = str + alignStx; *p; p++ )
                    if(( *p == ' ' ) || ( *p == '\t' ))
                        break;
            }
            else
                p++;

            alignStx += p - ( str + alignStx );
#ifdef DEBUG
            printf("alignStx %d, p %p\n", alignStx, p );
#endif
            if( str[ alignStx ] == 0 )
                break;
        }

        hch_alignStx( HMLELineQueryStr( this, 0 ), &alignStx );
        newLine = HMLELineSplit( this, alignStx );
        this->wordWrapped = TRUE;
        this = newLine;
    }
    this->wordWrapped = FALSE;

    return 0;
}

int HMLELineQueryWordWrapInfo( HMLELine *this, int size, BOOL wordProtect, int tabSize, int *stx )
{
    int len = 0;
    int lines = 1;
    int idx, cntStx, oldIdx;
    char *str;
    HMLELine *line;

    if( this == NULL ) return -1;
    if( size < 0 ) return -1;

    str = HMLELineQueryStr( this, 0 );
    line = HMLECreateLine( str );

    for( cntStx = 0, idx = 0; str[ idx ] != 0; cntStx ++, idx ++ )
    {
        switch( str[ idx ] )
        {
            case '\t':  len += queryTabspaceCol( line, len, tabSize );  break;
            default:    len ++;  break;
        }

        if( len > size )
        {
            if( wordProtect )
            {
                char *p;

#ifdef DEBUG
                printf("stx %d, cntStx %d, idx %d, str %p, len %d\n", *stx, cntStx, idx, str, strlen( str ));
#endif

                for( p = str + idx - 1; p > str; p-- )
                {
                    if(( *p == ' ' ) || ( *p == '\t' ))
                        break;
                }

                if(( *p != ' ' ) && ( *p != '\t' ))
                {
                    for( p = str + idx; *p; p++ )
                    {
                        if(( *p == ' ' ) || ( *p == '\t' ))
                            break;
                    }
                }
                else
                    p++;

                cntStx += p - ( str + idx );
                idx += p - ( str + idx );

#ifdef DEBUG
                printf("cntStx %d, idx %d, str %p, len %d\n", cntStx, idx, str, strlen( str ));
#endif

                if( !str[ idx ] )
                {
                    *stx = idx;
                    break;
                }
            }

            if( cntStx <= *stx ) lines ++;

            oldIdx = idx;
            hch_alignStx( str, &idx );
            cntStx -= oldIdx - idx;

            str += idx;
            idx = 0;
            len = ( str[ idx ] == '\t' ) ? tabSize : 1;

            HMLEDestroyLine( line );

            line = HMLECreateLine( str );
        }

        if( cntStx == *stx )
           *stx = idx;
    }

    if( cntStx == *stx )    // for insertion of non-LF terminated string
        *stx = idx;

    HMLEDestroyLine( line );

    return lines;
}

int HMLELineInsertPSLF_limit(HMLELine *this,int stx,PSLF str,int limit)
{
int newsize;
char *newstr;
int addLen,thisLen;

#ifdef DEBUG
    assert(this!=NULL);
    assert(str!=NULL);
#endif

    thisLen = strlen(this->str);
    if (stx>thisLen) return 0;
#ifdef DEBUG
    assert(stx<limit);
#endif

    addLen = PSLFstrlen(str);
    newsize = thisLen+addLen+1;
    if (newsize > limit)
        {
        if (isPSLF(str)) HMLELineSplit(this,stx);
        thisLen = strlen(this->str);
        newstr = malloc(limit);
        if (newstr==NULL) return 0;
        if (stx>0) memcpy(newstr,this->str,stx);
        memcpy(newstr+stx,str,(limit-1)-thisLen);
        newstr[limit-1] = '\0';
        free(this->str);
        this->str = newstr;
        this->allocsize = limit;
        if (isPSLF(str)) return -(limit-1-thisLen+1);
        return -(limit-1-thisLen);
        }

    newstr = malloc(newsize);
    if (newstr==NULL) return 0;
    if (stx>0) memcpy(newstr,this->str,stx);
    PSLFstrcpy(newstr+stx,str);
    PSLFtoPSZ(newstr);
    strcat(newstr,(this->str)+stx);

    free(this->str);
    this->str = newstr;
    this->allocsize = newsize;

    if (isPSLF(str))
        {
        HMLELineSplit(this,stx+addLen);
        return addLen+1;
        }
    return addLen;
}

int HMLELineInsertPSLF(HMLELine *this,int stx,PSLF str)
{
int newsize;
char *newstr;
int len,thisLen;

    if (this==NULL) return -1;
    if (str==NULL) return -1;
    thisLen = PSLFstrlen(this->str);
    if (stx>thisLen) return -1;

    len = PSLFstrlen(str);
    newsize = HMLELineQueryLen(this)+len+1;
    newstr = malloc(newsize);

    if (newstr==NULL) return -1;
    if (stx>0) memcpy(newstr,this->str,stx);
    PSLFstrcpy(newstr+stx,str);
    PSLFtoPSZ(newstr);
    strcat(newstr,(this->str)+stx);

    free(this->str);
    this->str = newstr;
    this->allocsize = newsize;

    if (isPSLF(str))
        {
        HMLELineSplit(this,stx+len);
        return len+1;
        }
    return len;
}

int HMLELineInsertHch(HMLELine *this,int stx,HANCHAR hch)
{
int tmp;
    if (this==NULL) return -1;
    if (stx>strlen(this->str)) return -1;
    if (this->allocsize < strlen(this->str)+3) return 0;

    tmp=hch_insertHchStx(this->str,hch,stx);
    return tmp;
}

HANCHAR HMLELineDeleteHch(HMLELine *this,int stx)
{
    if (this==NULL) return -1;
    if (stx>strlen(this->str)) return -1;

    return hch_deleteHchStx(this->str,stx);
}

int HMLELineDeleteFrom(HMLELine *this,int stx)
{
    if (this==NULL) return -1;
    if (stx>strlen(this->str)) return -1;

    this->str[stx]=0;
    return 0;
}

int HMLELineDeleteTo(HMLELine *this,int stx)
{
int newsize;
char *newstr;

    if (this==NULL) return -1;
    if (stx>strlen(this->str)) return -1;

    newsize = strlen(this->str+stx) +1;
    newstr = malloc(newsize);
    if (newstr==NULL) return -1;
    strcpy(newstr,this->str+stx);
    free(this->str);
    this->str=newstr;
    this->allocsize=newsize;
    return 0;
}

int HMLELineDeleteFromTo(HMLELine *this,int fromStx,int toStx)
{
int newsize;
char *newstr;

    if (this==NULL) return -1;
    if (fromStx>strlen(this->str)) return -1;
    if (toStx>strlen(this->str)) return -1;
    if (fromStx>toStx) return -1;
    if (fromStx==toStx) return 0;

    newsize = fromStx + strlen(this->str+toStx) +1;
    newstr = malloc(newsize);
    if (newstr==NULL) return -1;
    strncpy(newstr,this->str,fromStx);
    newstr[fromStx]=0;
    if (toStx<strlen(this->str)) strcat(newstr,this->str+toStx);
    free(this->str);
    this->str=newstr;
    this->allocsize=newsize;
    return 0;
}

int HMLELineNextWord(HMLELine* this,int stx,const char* separators)
{
    if (this==NULL) return -1;

    while ((this->str[stx]!='\0')&&(strchr(separators,this->str[stx])==NULL)) stx++;
    while ((this->str[stx]!='\0')&&(strchr(separators,this->str[stx])!=NULL)) stx++;
    return stx;
}

int HMLELinePrevWord(HMLELine* this,int stx,const char* separators)
{
    if (this==NULL) return -1;

    while ((stx>0)&&(strchr(separators,this->str[stx-1])!=NULL)) stx--;
    while ((stx>0)&&(strchr(separators,this->str[stx-1])==NULL)) stx--;
    return stx;
}

int HMLELineSearchStr(HMLELine* this,int stx,const char* str,int caseSens)
{
char *src=NULL, *searchStr=NULL;
int srcLen=0, searchLen=0;
char *find;

#ifdef DEBUG
//  printf("HMLELine::Search....");
    assert(this!=NULL);
    assert(this->str!=NULL);
    assert(str!=NULL);
//  printf("linestr = %x \n",this,this->str);
#endif

    if (this==NULL) return -1;
    srcLen = strlen(this->str);
    src = malloc(srcLen+1);
    if (src==NULL) return -1;

    searchLen = strlen(str);
    searchStr = malloc(searchLen+1);
    if (searchStr==NULL)
        {
        free(src);
        return -1;
        }

    strcpy(src,this->str);
    strcpy(searchStr,str);
    if (!caseSens)
        {
        hch_strlwr(src);
        hch_strlwr(searchStr);
        }

    find = strstr(src+stx,searchStr);

    free(src);
    free(searchStr);

    if (find==NULL) return -1;
    return ((int)find-(int)(src));
}

int queryTabspaceCol( HMLELine *this, int col, int tabSize )
{
    return( tabSize - ( col % tabSize ));
}

int queryLenWithTabSize( HMLELine *this, int tabSize )
{
int len = 0;
char *str;

    str = HMLELineQueryStr( this, 0 );

    while( *str != 0 )
    {
        switch( *str++ )
        {
            case '\t':  len += queryTabspaceCol( this, len, tabSize);  break;
            default:    len++;  break;
        }
    }

    return len;
}

int col2stx( HMLELine *this, int col, int tabSize )
{
int stx = 0;
int c = 0;
char *str;

    str = HMLELineQueryStr( this,0);

    while(( str[ stx ] !=0 ) && ( c < col ))
    {
        switch (str[stx])
        {
            case '\t':  c += queryTabspaceCol( this, c, tabSize );
                        if( c > col )
                            stx --;
                        break;

            default:    c ++;    break;
        }

        stx ++;
    }

    return stx;
}



