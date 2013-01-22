#include "HMLE_internal.h"

HMLEIpt* HMLEIptBefore(HMLEIpt* ipt1,HMLEIpt* ipt2)
{
	if (ipt1==NULL) return NULL;
	if (ipt2==NULL) return NULL;
	
	if (ipt1->ln < ipt2->ln) return ipt1;
	if (ipt1->ln > ipt2->ln) return ipt2;
	
	if (ipt1->stx < ipt2->stx) return ipt1;
	if (ipt1->stx > ipt2->stx) return ipt2;
	
	return ipt1;
}

HMLEIpt* HMLEIptAfter(HMLEIpt* ipt1,HMLEIpt* ipt2)
{
HMLEIpt* beforeIpt;

	beforeIpt = HMLEIptBefore(ipt1,ipt2);
	if (beforeIpt==NULL) return NULL;
	if (beforeIpt==ipt2) return ipt1;
	if (beforeIpt==ipt1) return ipt2;
	return NULL; // never
}

int HMLEIptCompare(HMLEIpt *this,HMLEIpt* ipt)
{
	if (this==NULL) return 0;
	if (ipt==NULL) return 0;
	
	if (this->ln==ipt->ln)
		{
		if (this->stx==ipt->stx) return 0;
		else if (this->stx>ipt->stx) return 1;
		else return -1;
		}
	if (this->ln > ipt->ln) return 1;
	if (this->ln < ipt->ln) return -1;
	
	return 0;	// never
}
