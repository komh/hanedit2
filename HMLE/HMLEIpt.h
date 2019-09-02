#ifndef __HMLEIPT_H__
#define __HMLEIPT_H__

typedef struct tagHMLEIpt HMLEIpt;
struct tagHMLEIpt {
	int ln;
	int stx;
};

HMLEIpt* HMLEIptBefore(HMLEIpt* ipt1,HMLEIpt* ipt2);
HMLEIpt* HMLEIptAfter(HMLEIpt* ipt1,HMLEIpt* ipt2);
int HMLEIptCompare(HMLEIpt* this,HMLEIpt *ipt);

#endif // __HMLEIPT_H__
