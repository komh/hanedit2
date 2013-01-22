#include <stdio.h>
#include <string.h>
#include "HMLE_internal.h"

#define HMLEDOC_DEFAULTSEPARATORS	(" \t;:,./\\\"\'()[]{}<>+-_|=")
#define HMLEDOC_MINIMALSEPARATORS	(" \t")

HMLEDoc* HMLECreateDoc(int cbEditBuf)
{
HMLEDoc* doc;
HMLELine* line;

	doc = malloc(sizeof(HMLEDoc));
	if (doc == NULL) return NULL;

	doc->curStx = 0L;
	doc->anchorLine = NULL;
	doc->anchorStx = 0L;
	doc->markingState = 0L;
	doc->editBufSize = cbEditBuf;
	doc->hchComposing = 0;
	doc->tabsize = 8;
	doc->changed = FALSE;
	doc->separators = NULL;
	
	HMLEDocSetSeparators(doc,HMLEDOC_DEFAULTSEPARATORS);

	line = HMLECreateLine("\0");
	if (line==NULL)
		{
		free(doc);
		return NULL;
		}
	doc->beginLine = doc->endLine = doc->curLine = line;

	HMLEDocUnpackCurLine(doc);

#ifdef DEBUG
//	dump(stderr,"HMLEDoc",sizeof(HMLEDoc),(char*)doc);
//	dump(stderr,"beginLine",sizeof(HMLELine),(char*)line);
#endif

	return doc;
}

void HMLEDestroyDoc(HMLEDoc* this)
{
HMLELine *line;
HMLELine *nextLine;

	if (this==NULL) return;

	line=this->beginLine;
	while (line!=NULL)
		{
		nextLine = line->nextLine;
		HMLEDestroyLine(line);
		line = nextLine;
		}
	if (this->separators!=NULL) free(this->separators);
	free(this);
}

void HMLEDocSetSeparators(HMLEDoc* this,const char* separators)
{
	if (this==NULL) return;
	if (separators==NULL) return;
	if (strlen(separators)==0)
		{
		HMLEDocSetSeparators(this,HMLEDOC_MINIMALSEPARATORS);
		return;
		}

	if (this->separators!=NULL) free(this->separators);
	this->separators = malloc(strlen(separators)+1);
	strcpy(this->separators,separators);
}

int HMLEDocInsertTextThunk(HMLEDoc* this,HMLETextThunk *textThunk)
{
int ln,lines;
int ret;

char *pch = NULL;

	this->errno = 0;

	if (this==NULL) this->errno = HMLEDOC_ERROR_NULLINSTANCE;
	if (textThunk==NULL) this->errno = HMLEDOC_ERROR_INVALIDARG;
	if (this->errno) return -1;

	lines = HMLETextThunkQueryNumberOfLines(textThunk);
	HMLETextThunkRewind(textThunk);
	ln=0;
	pch = HMLETextThunkQueryCurLine(textThunk);
	if (ln<lines-1)
		{
		ret = HMLELineInsertPSLF_limit(
			this->curLine,this->curStx,pch,this->editBufSize);
		if (ret<0) this->errno = HMLEDOC_ERROR_INSERTIONTRUNCATED;
		HMLEDocMoveCurLineNext(this);
		this->curStx=0;
		ln++;
		pch = HMLETextThunkNextLine(textThunk);
		}
	for (;ln<lines-1;ln++,pch = HMLETextThunkNextLine(textThunk))
		{
		if (pch==NULL) return -1;

		ret = HMLELineInsertPSLF_limit(this->curLine,0,pch,this->editBufSize);
		if (ret<0) this->errno = HMLEDOC_ERROR_INSERTIONTRUNCATED;
		HMLEDocMoveCurLineNext(this);
		}
	if (ln==lines-1)
		{
		ret = HMLELineInsertPSLF_limit(
			this->curLine,this->curStx,pch,this->editBufSize);
		if (ret>0)
			{
			this->curStx+=ret;		// fully inserted
			} else {
			this->curStx-=ret;		// truncated
			this->errno = HMLEDOC_ERROR_INSERTIONTRUNCATED;
			}
		}

	if (this->errno!=0) return -1;
	return 0;
}

HMLETextThunk* HMLEDocCreateTextThunk(HMLEDoc* this,HMLEIpt *ipt1,HMLEIpt *ipt2)
{
HMLETextThunk* textThunk;
HMLEIpt* beginIpt;
HMLEIpt* endIpt;
HMLELine *line;
int ln;

char *newstr;
int newsize;

	if (this==NULL) return NULL;
	if (ipt1==NULL) return NULL;
	if (ipt2==NULL) return NULL;
	
	beginIpt = HMLEIptBefore(ipt1,ipt2);
	endIpt = HMLEIptAfter(ipt1,ipt2);
	textThunk = HMLECreateTextThunk("\0");
	newsize = HMLEDocQuerySize(this,beginIpt,endIpt)+1;
//	printf("DocCreateTextThunkSize = %d\n",newsize);
	HMLETextThunkPresetSize(textThunk,newsize);

	ln = beginIpt->ln;
	line = HMLEDocQueryLine(this,beginIpt->ln);
	if (ln!=endIpt->ln)
		{
		HMLETextThunkAddLine(textThunk,HMLELineQueryStr(line,beginIpt->stx));
		line = line->nextLine;
		ln++;
		}
	for (;ln<endIpt->ln;ln++,line=line->nextLine)
		HMLETextThunkAddLine(textThunk,HMLELineQueryStr(line,0));

/*	for (ln = beginIpt->ln; ln < endIpt->ln; ln++)
		{
		line = HMLEDocQueryLine(this,ln);
		if (line==NULL) return NULL;
		
		if (ln==beginIpt->ln)
			HMLETextThunkAddLine(textThunk,HMLELineQueryStr(line,beginIpt->stx));
			else
			HMLETextThunkAddLine(textThunk,HMLELineQueryStr(line,0));
		} */
//	if (ln==endIpt->ln)
	if (ln == endIpt->ln)
		{
//		line = HMLEDocQueryLine(this,ln);
//		if (line==NULL) return NULL;
		
		newsize = HMLELineQueryLen(line)+1;
		newstr = malloc(newsize);
		strcpy(newstr,HMLELineQueryStr(line,0));
		*(newstr+(endIpt->stx)) = '\0';
		if (ln==beginIpt->ln)
			HMLETextThunkAddStr(textThunk,newstr+(beginIpt->stx));
			else
			HMLETextThunkAddStr(textThunk,newstr);
		free(newstr);
		}
	return textThunk;
}

int HMLEDocDelete(HMLEDoc *this,HMLEIpt *beginIpt,HMLEIpt *endIpt)
{
HMLELine *line;
HMLELine *beginLine;
HMLELine *endLine;

int selectionSize = 0;

	if (this==NULL) return -1;
	if (this->markingState==0) return 0;

	beginLine = HMLEDocQueryLine(this,beginIpt->ln);
	endLine = HMLEDocQueryLine(this,endIpt->ln);
	if ((beginLine==NULL)||(endLine==NULL)) return -1;

	selectionSize = HMLEDocQuerySize(this,beginIpt,endIpt);

	line = beginLine;
	while (line!=endLine)
		{
		if (line==beginLine)
			{
			HMLELineDeleteFrom(line,beginIpt->stx); // in-bound
			line=line->nextLine;
			} else {
			line=line->nextLine;
			HMLEDestroyLine(line->prevLine);
			}
		}
	if (line==endLine)
		{
		if (line==beginLine)
			{
			HMLELineDeleteFromTo(line,beginIpt->stx,endIpt->stx);
			} else {
			HMLELineDeleteTo(line,endIpt->stx); // ex-bound

			HMLEDocSetCurIpt(this,beginIpt);
			if (HMLEDocCombineLines(this)!=0) return -1;
			}
		} else return -1;

	HMLEDocSetCurIpt(this,beginIpt);

	this->changed=TRUE;
	return selectionSize;
}

int HMLEDocQuerySize(HMLEDoc* this,HMLEIpt *ipt1,HMLEIpt *ipt2)
{
HMLEIpt* beginIpt;
HMLEIpt* endIpt;
HMLELine *line;
int ln;

int size = 0;

	if (this==NULL) return -1;
	if (ipt1==NULL) return -1;
	if (ipt2==NULL) return -1;
	
	beginIpt = HMLEIptBefore(ipt1,ipt2);
	endIpt = HMLEIptAfter(ipt1,ipt2);

	ln = beginIpt->ln;
	line = HMLEDocQueryLine(this,beginIpt->ln);
	for (;ln<=endIpt->ln;ln++,line=line->nextLine)
		{
		size+=(HMLELineQueryLen(line)+2);
		}

	return size;
}

HMLELine* HMLEDocQueryLine(HMLEDoc* this,int ln)
{
	if (this==NULL) return NULL;
	
	return HMLELineQueryLine_NullSafe(this->beginLine,ln);
}

HMLELine *HMLEDocQueryEndLine(HMLEDoc *this)
{
HMLELine *line;

	if (this==NULL) return NULL;

	line = this->beginLine;
	if (line==NULL) return NULL;

	while (line->nextLine!=NULL)
		line=line->nextLine;
	this->endLine = line;

	return this->endLine;
}

int HMLEDocQueryNumberOfLines(HMLEDoc *this)
{
int n = 0;
HMLELine *line;

	if (this==NULL) return -1;
	if (this->beginLine == NULL) return -1;

	line = this->beginLine;
	while (line!=NULL)
		{
		n++;
		line=line->nextLine;
		}

	return n;
}

int HMLEDocPackCurLine(HMLEDoc* this)
{
	if (this==NULL) return -1;
	
	HMLELinePack(this->curLine);
	return 0;
}

int HMLEDocUnpackCurLine(HMLEDoc* this)
{
	if (this==NULL) return -1;
	
	HMLELineUnpack(this->curLine,this->editBufSize);
	return 0;
}

HMLELine* HMLEDocMoveCurLineTo(HMLEDoc* this,int idx)
{
int oldCol,stx;
HMLELine* line;

	if (this==NULL) return NULL;

	oldCol = HMLEDocStx2Col(this,this->curLine,this->curStx);
	line = HMLEDocQueryLine(this,idx);
	if (line==NULL) return NULL;

	HMLEDocPackCurLine(this);
	this->curLine = line;
	HMLEDocUnpackCurLine(this);

	stx = HMLEDocCol2Stx(this,this->curLine,oldCol);
	HMLEDocMoveCurStxTo(this,stx);


	return (this->curLine);
}

int HMLEDocMoveCurLineNext(HMLEDoc *this)
{
int oldCol,stx;

	if (this==NULL) return 0;

	if (this->curLine->nextLine==NULL) return 0;

	oldCol = HMLEDocStx2Col(this,this->curLine,this->curStx);

	HMLEDocPackCurLine(this);
	this->curLine = this->curLine->nextLine;
	HMLEDocUnpackCurLine(this);

	stx = HMLEDocCol2Stx(this,this->curLine,oldCol);
	HMLEDocMoveCurStxTo(this,stx);

	return 1;
}

int HMLEDocMoveCurLinePrev(HMLEDoc *this)
{
int oldCol,stx;

	if (this==NULL) return 0;

	if (this->curLine->prevLine==NULL) return 0;

	oldCol = HMLEDocStx2Col(this,this->curLine,this->curStx);

	HMLEDocPackCurLine(this);
	this->curLine = this->curLine->prevLine;
	HMLEDocUnpackCurLine(this);

	stx = HMLEDocCol2Stx(this,this->curLine,oldCol);
	HMLEDocMoveCurStxTo(this,stx);
	return -1;
}

int HMLEDocMoveCurLine(HMLEDoc* this,int step)
{
int lnc = 0;

	if (this==NULL) return 0;

	while (step>0)
		{
		lnc += HMLEDocMoveCurLineNext(this);
		step --;
		}
	while (step<0)
		{
		lnc += HMLEDocMoveCurLinePrev(this);
		step ++;
		}
	return lnc;
}

int HMLEDocMoveCurStxTo(HMLEDoc* this,int stx)
{
int len;
int tmpStx = stx;

	if (this==NULL) return -1;
	if (stx<0) return -1;

	len = HMLELineQueryLen(this->curLine);
	if (tmpStx > len) tmpStx = len;
	
	hch_alignStx(HMLELineQueryStr(this->curLine,0),&tmpStx);
	if (tmpStx>=0) this->curStx = tmpStx;
	
	return 0;
}

int HMLEDocMoveCurStx(HMLEDoc *this,int step)
{
	if (this==NULL) return 0;
	if (step==0) return 0;

	while (step<0)
		{
		step++;
		if (this->curStx > 0)
			this->curStx --;
		else if (this->curLine != this->beginLine)
			{
			HMLEDocMoveCurLinePrev(this);
			this->curStx = HMLELineQueryLen(this->curLine);
			} else step = 0;
		}
	while (step>0)
		{
		step--;
		if (this->curStx < HMLELineQueryLen(this->curLine))
			this->curStx ++;
		else if (this->curLine->nextLine != NULL)
			{
			HMLEDocMoveCurLineNext(this);
			this->curStx = 0;
			} else step = 0;
		}

	return this->curStx;
}

int HMLEDocMoveCurStxLeft(HMLEDoc *this,int step)
{
int tempStx;
	if (this==NULL) return -1;
	if (step==0) return this->curStx;
	
	while (step>0)
		{
		step--;
		tempStx = this->curStx;
		if (hch_decStx(HMLELineQueryStr(this->curLine,0),&tempStx))
			{
			this->curStx = tempStx;
			} else {
			if (HMLEDocMoveCurLinePrev(this))
				this->curStx = HMLELineQueryLen(this->curLine);
			}
		}
	return this->curStx;
}

int HMLEDocMoveCurStxRight(HMLEDoc *this,int step)
{
int tempStx;

	if (this==NULL) return -1;
	if (step==0) return this->curStx;
	
	while (step>0)
		{
		step--;
		tempStx = this->curStx;
		if (hch_incStx(HMLELineQueryStr(this->curLine,0),&tempStx))
			{
			this->curStx = tempStx;
			} else {
			if (HMLEDocMoveCurLineNext(this))
				this->curStx = 0;
			}
		}
	return this->curStx;
}

int HMLEDocMoveNextWord(HMLEDoc *this)
{
int tempStx;

	if (this==NULL) return -1;
	if (this->curStx == HMLELineQueryLen(this->curLine))
		{
		HMLEDocMoveCurLineNext(this);
		HMLEDocMoveCurStxTo(this,0);
		return 0;
		}
	tempStx = HMLELineNextWord(this->curLine,this->curStx,this->separators);
	if (tempStx==-1) return -1;
	HMLEDocMoveCurStxTo(this,tempStx);
	return 0;
}

int HMLEDocMovePrevWord(HMLEDoc *this)
{
int tempStx;

	if (this==NULL) return -1;
	if (this->curStx == 0)
		{
		HMLEDocMoveCurLinePrev(this);
		HMLEDocMoveCurStxTo(this,HMLELineQueryLen(this->curLine));
		return 0;
		}
	tempStx = HMLELinePrevWord(this->curLine,this->curStx,this->separators);
	if (tempStx==-1) return -1;
	HMLEDocMoveCurStxTo(this,tempStx);
	return 0;
}

HMLELine* HMLEDocSplitCurLine(HMLEDoc* this)
{
HMLELine* newLine;

	if (this==NULL) return NULL;
	if (this->curLine==NULL) return NULL;

	newLine = HMLELineSplit(this->curLine,this->curStx);
	if (this->endLine == this->curLine) this->endLine = newLine;
	HMLEDocMoveCurLineNext(this);
	if (this->curLine->nextLine==NULL) this->endLine = this->curLine;
//	HMLEDocMoveCurLineTo(this,HMLELineQueryLineNumber(this->curLine)+1);
	HMLEDocMoveCurStxTo(this,0);
	this->changed = TRUE;
	return newLine;
}

int HMLEDocCombineLines(HMLEDoc *this)
{
	if (this==NULL) return -1;
	if (this->curLine==NULL) return -1;
	if (this->curStx != HMLELineQueryLen(this->curLine))
		return -1;

	if (this->curLine->nextLine==NULL) return 0;
	HMLELineCombineWithNextLine(this->curLine);
	if (this->curLine->nextLine==NULL) this->endLine = this->curLine;
	HMLEDocUnpackCurLine(this);
	this->changed = TRUE;
	return 0;
}

int HMLEDocQueryOfs(HMLEDoc *this,HMLEIpt *ipt)
{
HMLELine *line;
int ln;
int ofs=0;

	if (this==NULL) return -1;
	if (this==NULL) return -1;
	
	for (ln=0,line=this->beginLine;(ln<=ipt->ln)&&(line!=NULL);ln++,line=line->nextLine)
		{
		if (ln==ipt->ln)
			{
			ofs+=ipt->stx;
			return ofs;
			} else {
			ofs += (HMLELineQueryLen(line)+1);
			}
		}
	return ofs;
}

int HMLEDocQueryIpt(HMLEDoc *this,HMLEIpt *ipt,int ofs)
{
HMLELine *line;
int ofsbuf = ofs;
int len;

	if (this==NULL) return -1;
	if (ipt==NULL) return -1;

	ipt->ln = 0;
	line = this->beginLine;
	len = HMLELineQueryLen(line);

	while ((line != NULL) && (ofsbuf > len))
		{
		ofsbuf -= (len);
		line = line->nextLine;
		if (line==NULL)
			{
			if (ofsbuf>0)
				{
				printf("ofsbuf=%d\n",ofsbuf);
				return -1;
				}
			ipt->stx = ofsbuf;
			return 0;
			}
		ofsbuf--;
		len = HMLELineQueryLen(line);
		ipt->ln++;
		}
	ipt->stx = ofsbuf;

	return 0L;
}

int HMLEDocQueryAnchorIpt(HMLEDoc *this,HMLEIpt *ipt)
{

	if (this==NULL) return -1;
	
	ipt->ln = HMLELineQueryLineNumber(this->anchorLine);
	ipt->stx = this->anchorStx;

	return 0L;
}

int HMLEDocQueryCurIpt(HMLEDoc *this,HMLEIpt *ipt)
{

	if (this==NULL) return -1;
	
	ipt->ln = HMLELineQueryLineNumber(this->curLine);
	ipt->stx = this->curStx;

	return 0L;
}

int HMLEDocSetCurIpt(HMLEDoc *this,HMLEIpt *ipt)
{
	if (this==NULL) return -1;

	HMLEDocPackCurLine(this);
	HMLEDocMoveCurLineTo(this,ipt->ln);
	HMLEDocMoveCurStxTo(this,ipt->stx);
	HMLEDocUnpackCurLine(this);

/*	if (this->curStx > HMLEQueryLineLen(this->curLine))
		this->curStx = HMLEQueryLineLen(this->curLine); */
	return 0;
}

int HMLEDocQuerySelectionBeginIpt(HMLEDoc *this,HMLEIpt *ipt)
{
	if (this->markingState==0)
		{
		HMLEDocQueryCurIpt(this,ipt);
		} else {
		HMLEIpt curIpt,anchorIpt;
		HMLEIpt *minIpt;
		
		HMLEDocQueryCurIpt(this,&curIpt);
		HMLEDocQueryAnchorIpt(this,&anchorIpt);
		minIpt = HMLEIptBefore(&curIpt,&anchorIpt);
		ipt->ln = minIpt->ln;
		ipt->stx = minIpt->stx;
		}
	return 0;
}

int HMLEDocQuerySelectionEndIpt(HMLEDoc *this,HMLEIpt *ipt)
{
	if (this->markingState==0)
		{
		HMLEDocQueryCurIpt(this,ipt);
		} else {
		HMLEIpt curIpt,anchorIpt;
		HMLEIpt *maxIpt;
		
		HMLEDocQueryCurIpt(this,&curIpt);
		HMLEDocQueryAnchorIpt(this,&anchorIpt);
		maxIpt = HMLEIptAfter(&curIpt,&anchorIpt);
		ipt->ln = maxIpt->ln;
		ipt->stx = maxIpt->stx;
		}
	return 0;
}

int HMLEDocInsertHch(HMLEDoc *this,HANCHAR hch)
{
int inc;

	if (this==NULL) return -1;

	if (hch=='\n')
		{
		this->changed = TRUE;
		HMLEDocSplitCurLine(this);
		return 0;
		}

	if (HMLELineIsPacked(this->curLine))
		HMLEDocUnpackCurLine(this);

	inc=HMLELineInsertHch(this->curLine,this->curStx,hch);

	if (inc>0) this->changed = TRUE;
#ifdef DEBUG
	if ((inc>2)||(inc<1)) printf("HMLEDocInsertHch: inc=%d\n",inc);
#endif
	HMLEDocMoveCurStx(this,inc);
	return 0;
}

HANCHAR HMLEDocDeleteHch(HMLEDoc *this)
{
	if (this==NULL) return -1;

	this->changed = TRUE;
	if (this->curStx == HMLELineQueryLen(this->curLine))
		{
		HMLEDocCombineLines(this);
		return '\n';
		} else {
		return HMLELineDeleteHch(this->curLine,this->curStx);
		}

}

int HMLEDocBeginSelectionHere(HMLEDoc *this,HMLEIpt *ipt)
{

	this->markingState = HMLEDOC_MARKING_BEGIN;
	this->anchorLine = HMLEDocQueryLine(this,ipt->ln);
	this->anchorStx = ipt->stx;

	return 0L;
}

int HMLEDocEndSelection(HMLEDoc *this)
{

	this->markingState = HMLEDOC_MARKING_NONE;

	return 0L;
}

int HMLEDocQueryMarkingState(HMLEDoc *this)
{
	return this->markingState;
}

HMLETextThunk* HMLEDocGetSelection(HMLEDoc *this)
{
HMLEIpt anchorIpt;
HMLEIpt curIpt;
HMLETextThunk *textThunk;

	if (this==NULL) return NULL;
	if (this->markingState==0) return NULL;

	HMLEDocQueryAnchorIpt(this,&anchorIpt);
	HMLEDocQueryCurIpt(this,&curIpt);

	textThunk = HMLEDocCreateTextThunk(this,&anchorIpt,&curIpt);
	return textThunk;
}

int HMLEDocDeleteSelection(HMLEDoc *this)
{
HMLEIpt anchorIpt;
HMLEIpt curIpt;
HMLEIpt *beginIpt;
HMLEIpt *endIpt;

HMLELine *line;
HMLELine *beginLine;
HMLELine *endLine;

int selectionSize = 0;

	if (this==NULL) return -1;
	if (this->markingState==0) return 0;

	HMLEDocQueryAnchorIpt(this,&anchorIpt);
	HMLEDocQueryCurIpt(this,&curIpt);
	beginIpt = HMLEIptBefore(&anchorIpt,&curIpt);
	endIpt = HMLEIptAfter(&anchorIpt,&curIpt);

	beginLine = HMLEDocQueryLine(this,beginIpt->ln);
	endLine = HMLEDocQueryLine(this,endIpt->ln);
	if ((beginLine==NULL)||(endLine==NULL)) return -1;

	selectionSize = HMLEDocQuerySize(this,&anchorIpt,&curIpt);

	line = beginLine;
	while (line!=endLine)
		{
		if (line==beginLine)
			{
			HMLELineDeleteFrom(line,beginIpt->stx); // in-bound
			line=line->nextLine;
			} else {
			line=line->nextLine;
			HMLEDestroyLine(line->prevLine);
			}
		}
	if (line==endLine)
		{
		if (line==beginLine)
			{
			HMLELineDeleteFromTo(line,beginIpt->stx,endIpt->stx);
			} else {
			HMLELineDeleteTo(line,endIpt->stx); // ex-bound

			HMLEDocEndSelection(this);
			HMLEDocSetCurIpt(this,beginIpt);
			if (HMLEDocCombineLines(this)!=0) return -1;
			}
		} else return -1;

	HMLEDocSetCurIpt(this,beginIpt);
	HMLEDocEndSelection(this);

	this->changed=TRUE;
	return selectionSize;
}

int HMLEDocStreamOut(HMLEDoc* this,FILE *stream)
{
HMLELine* line;

	if (stream==NULL) return -1;
	if (this==NULL) return -1;

	line = this->beginLine;

	while(line!=NULL)
		{
		fprintf(stream,"%s\n",line->str);
		line = line->nextLine;
		}
	return 0;
}

int HMLEDocReport(HMLEDoc* this,FILE *stream)
{
HMLELine* line;

	if (stream==NULL) return -1;
	if (this==NULL) return -1;

	line = this->beginLine;

	while(line!=NULL)
		{
		fprintf(stream,"(%3d,%3d,%3d)%s\n",line->allocsize,PSLFstrlen(line->str),strlen(line->str),line->str);
		line = line->nextLine;
		}
	return 0;
}

/* ULONG HMLEDocExportSelection(HMLEDoc *this,char *buf)
{
HMLELine *Line;
HMLEipt beginIPT,endIPT;
ULONG idx = 0;
char *str;

	assert(this!=NULL);
	if (this->markingState ==0) return 0;

	HMLEDocQueryBeginIPT(this,&beginIPT);
	HMLEDocQueryEndIPT(this,&endIPT);

	if (beginIPT.Line == endIPT.Line)
		{
		str = HpAllocBlock(this->docheap,strlen(beginIPT.Line->str)+1);
			strcpy(str,beginIPT.Line->str); str[endIPT.Stx] = 0;
			idx+=sprintf(buf,"%s",str+(beginIPT.Stx));
			buf[idx]=0;
		HpFreeBlock(this->docheap,str,strlen(beginIPT.Line->str)+1);
		return idx;
		}

	Line = beginIPT.Line;
	idx += sprintf(buf+idx,"%s",Line->str+(beginIPT.Stx));

	while ((Line = Line->nextLine)!=endIPT.Line)
		{
		assert(Line!=NULL);
		buf[idx++] = '\n';
		idx += sprintf(buf+idx,"%s",Line->str);
		//Line = Line->nextLine;
		}

	assert(Line == endIPT.Line);
	str = HpAllocBlock(this->docheap,strlen(Line->str)+1);
		strcpy(str,Line->str); str[endIPT.Stx] = 0;
		buf[idx++] = '\n';
		idx += sprintf(buf+idx,"%s",str);
		buf[idx] = 0;
	HpFreeBlock(this->docheap,str,strlen(Line->str)+1);

	return idx;
} */

void HMLEDocCheckPacking(HMLEDoc *this)
{
HMLELine *line;

	if (this==NULL) return;
	line = this->beginLine;
	while (line!=NULL)
		{
		if (!HMLELineIsPacked(line))
			if (line!=this->curLine)
				{
				printf("error:%d:not packed.",HMLELineQueryLineNumber(line));
				HMLELinePack(line);
				} else {
				HMLEDocPackCurLine(this);
				HMLEDocUnpackCurLine(this);
				}
		line = line->nextLine;
		}
}

void HMLEDocDump(HMLEDoc *this,FILE *stream,int options)
{
#ifdef DEBUG
	assert(this!=NULL);
	assert(this->beginLine!=NULL);
#endif

#ifdef DEBUG
	dump32(stderr,"HMLEDoc",sizeof(HMLEDoc),this);
	dump32(stderr,"beginLine",sizeof(HMLELine),this->beginLine);
	dump(stderr,"beginLine->str",this->beginLine->allocsize,this->beginLine->str);
#endif
}

int HMLEDocQueryTabsizeCol(HMLEDoc *this,int col)
{
	return this->tabsize;
}

int HMLEDocQueryTabspaceCol(HMLEDoc *this,int col)
{
int tabsize;

	tabsize = HMLEDocQueryTabsizeCol(this,col);
	return (tabsize - (col % tabsize));
}

int HMLEDocQueryTabsizeStx(HMLEDoc *this,HMLELine *line,int stx)
{
int col;

	col = HMLEDocStx2Col(this,line,stx);
	return HMLEDocQueryTabsizeCol(this,col);
}

int HMLEDocQueryTabspaceStx(HMLEDoc *this,HMLELine *line,int stx)
{
int col;

	col = HMLEDocStx2Col(this,line,stx);
	return HMLEDocQueryTabspaceCol(this,col);
}

int HMLEDocStx2Col(HMLEDoc *this,HMLELine *line,int stx)
{
int idx = 0;
int col = 0;
char *str;

	str = HMLELineQueryStr(line,0);

	while ((str !=0 ) && (idx < stx))
		{
		switch (str[idx])
		{
		case '\t':	col+=HMLEDocQueryTabspaceCol(this,col);	 break;
		default:	col++;	break;
		}
		idx ++;
		}
	return col;
}

int HMLEDocCol2Stx(HMLEDoc *this,HMLELine *line,int col)
{
int stx = 0;
int c = 0;
char *str;

	str = HMLELineQueryStr(line,0);

	while ((str[stx] !=0 ) && (c < col))
		{
		switch (str[stx])
		{
		case '\t':	c+=HMLEDocQueryTabspaceCol(this,c);	 break;
		default:	c++;	break;
		}
		stx ++;
		}
	return stx;
}

int HMLEDocFormatLine(HMLEDoc *this,HMLELine *line,char *str)
{
char *src,*dst;
int i = 0;
int col = 0,spaces = 0;
char *srcbuf = NULL;
int srcbufsize = 0;

	if (this==NULL) return -1;
	if (line==NULL) return -1;
	if (str==NULL) return -1;

	if ((line==this->curLine) && (this->hchComposing))
		{
		srcbufsize = HMLELineQueryLen(this->curLine) + 1 +2; // 1 for eol,2 for hch
		srcbuf = malloc(srcbufsize);
		if (srcbuf==NULL) return -1;
		strcpy(srcbuf,HMLELineQueryStr(this->curLine,0));
		hch_insertHchStx(srcbuf,this->hchComposing,this->curStx);
		src = srcbuf;

		} else {
		src = HMLELineQueryStr(line,0);
		}

	dst = str;

	while (*src != 0)
		{

		switch (*src)
		{
		case '\t':
			spaces = HMLEDocQueryTabspaceCol(this,col);
			for (i=0;i<spaces;i++,col++) *(dst++) = ' ';
			src++;
			break;
		default:
			*(dst++) = *(src++);
			col++;
			break;
		} // switch

		} // while
		*dst = 0;

	if (srcbuf != NULL) free(srcbuf);

	return 0L;
}

int HMLEDocFormatLine2(HMLEDoc *this,HMLELine *line,char *str,int fixedLen)
{
int i;

	if (HMLEDocFormatLine(this,line,str)==-1) return -1;

	fixedLen++;
	for (i=strlen(str);i<=fixedLen;i++)
		str[i]=' ';
	hch_alignStx(str,&fixedLen);
	str[fixedLen]=0;

	return 0;
}

int HMLEDocFindFirst(HMLEDoc *this,const char* text,int caseSens)
{
HMLELine *line = NULL;
int ln=0;
int findStx,stx;

#ifdef DEBUG
	printf("HMLEDoc:: FindFirst\n");
	printf("searchText = %s\n",text);
	assert(this!=NULL);
	assert(text!=NULL);
#endif

	if (this==NULL) return -1;
	if (text==NULL) return -1;
	
	line = this->beginLine;
	ln = 0;
	stx = 0;
	while (line!=NULL)
		{
		findStx = HMLELineSearchStr(line,stx,text,caseSens);
		if (findStx>=0)
			{
			HMLEDocMoveCurLineTo(this,ln);
			HMLEDocMoveCurStxTo(this,findStx);
			printf("%d,%d\n",ln,findStx);
			return TRUE;
			}
		line = line->nextLine;
		ln++;
		stx = 0;
		}
	printf("cat\'t find it\n");
	return 0;
}

int HMLEDocFindNext(HMLEDoc *this,const char* text,int caseSens)
{
HMLELine *line = NULL;
int ln=0;
int findStx,stx;

#ifdef DEBUG
	printf("HMLEDoc:: FindNext\n");
	printf("searchText = %s\n",text);
	assert(this!=NULL);
	assert(text!=NULL);
#endif

	if (this==NULL) return -1;
	if (text==NULL) return -1;
	
	line = this->curLine;
	ln=HMLELineQueryLineNumber(this->curLine);
	stx = this->curStx+1;
	while (line!=NULL)
		{
		findStx = HMLELineSearchStr(line,stx,text,caseSens);
		if (findStx>=0)
			{
			HMLEDocMoveCurLineTo(this,ln);
			HMLEDocMoveCurStxTo(this,findStx);
			return TRUE;
			}
		line = line->nextLine;
		ln++;
		stx = 0;
		}
	return 0;
}

int HMLEDocFindString(HMLEDoc *this,const char* findStr,
	HMLEIpt *beginIpt,HMLEIpt *endIpt,HMLEIpt *foundIpt,
	int caseSens)
{
HMLELine *line = NULL;
int ln=0;
int findStx,stx;

#ifdef DEBUG
	printf("HMLEDoc:: FindString\n");
	printf("searchText = %s\n",findStr);
	assert(this!=NULL);
	assert(findStr!=NULL);
	assert(beginIpt!=NULL);
	assert(endIpt!=NULL);
	assert(foundIpt!=NULL);
#endif

	if (this==NULL) return -1;
	if (findStr==NULL) return -1;
	
	ln = beginIpt->ln;
	stx = beginIpt->stx;
	line = HMLEDocQueryLine(this,ln);
	while ((line!=NULL) && (ln<=endIpt->ln))
		{
		findStx = HMLELineSearchStr(line,stx,findStr,caseSens);
		if (findStx>=0)
			{
			if ((ln==endIpt->ln) && (findStx+strlen(findStr)>endIpt->stx))
				return FALSE;
			foundIpt->ln = ln;
			foundIpt->stx = findStx;
			return TRUE;
			}
		line = line->nextLine;
		ln++;
		stx = 0;
		}
	return FALSE;
}
