#include "hin.h"
#include "hin_internal.h"

#define HIA_KEY_CONSONANT	0
#define HIA_KEY_VOWEL		1
#define HKEYKIND(a) ((((a) & 0x60)==0x20)?HIA_KEY_VOWEL:HIA_KEY_CONSONANT)

#define HKEYTOFST(key)		(((key)-0x80))	//<<10)
#define HKEYTOMID(key)		(((key)-0xA0))	//<<5)
#define HKEYTOLST(key)		(tblFSTTOLST[(key)-0x82]-0xC0)
#define HKEYCANBELST(key)	(tblFSTTOLST[(key)-0x82])

static unsigned char tblFSTTOLST[19] =
	{	  /* ÃÊ¼º ÄÚµå¿¡ ´ëÀÀÇÏ´Â Á¾¼º ÄÚµå */
	0xc2,	/*	±â¿ª				*/
	0xc3,	/*	½Ö±â¿ª				*/
	0xc5,	/*	´ÏÀº				*/
	0xc8,	/*	µð±Ú				*/
	0x00,	/*	½Öµð±Ú (ÇØ´ç ¾øÀ½)	*/
	0xc9,	/*	¸®À»				*/
	0xd1,	/*	¹ÌÀ½				*/
	0xd3,	/*	ºñÀ¾				*/
	0x00,	/*	»óºñÀ¾ (ÇØ´ç ¾øÀ½)	*/
	0xd5,	/*	½Ã¿Ê				*/
	0xd6,	/*	½Ö½Ã¿Ê				*/
	0xd7,	/*	ÀÌÀÀ				*/
	0xd8,	/*	ÁöÀÁ				*/
	0x00,	/*	½ÖÁöÀÁ (ÇØ´ç ¾øÀ½)	*/
	0xd9,	/*	Ä¡ÀÂ				*/
	0xda,	/*	Å°ÀÃ				*/
	0xdb,	/*	Æ¼ÀÄ				*/
	0xdc,	/*	ÇÇÀÅ				*/
	0xdd	/*	È÷ÀÆ				*/
	};

static HANCHAR HIAMakeDFST2(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[5][3] = {
	{0x82, 0x82, 0x83},		/* ‹¡µb, ‹¡µb, °w‹¡µb */
	{0x85, 0x85, 0x86},		/* —¡‹h, —¡‹h, °w—¡‹h */
	{0x89, 0x89, 0x8a},		/* §¡·s, §¡·s, °w§¡·s */
	{0x8b, 0x8b, 0x8c},		/* ¯¡µµ, ¯¡µµ, °w¯¡µµ */
	{0x8e, 0x8e, 0x8f}		/* »¡·x, »¡·x, °w»¡·x */
};
int i;

	for (i=0; i<5; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHFST(hch,HKEYTOFST(table[i][2])));

return 0;
}


static HANCHAR HIAMakeDMID2(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[7][3] = {
		{0xad, 0xa3, 0xae},       /* ¿À, ¾Æ, ¿Í */
		{0xad, 0xa4, 0xaf},       /* ¿À, ¾Ö, ¿Ö */
		{0xad, 0xbd, 0xb2},       /* ¿À, ÀÌ, ¿Ü */
		{0xb4, 0xa7, 0xb5},       /* ¿ì, ¾î, ¿ö */
		{0xb4, 0xaa, 0xb6},       /* ¿ì, ¿¡, ¿þ */
		{0xb4, 0xbd, 0xb7},       /* ¿ì, ÀÌ, À§ */
		{0xbb, 0xbd, 0xbc}		  /* À¸, ÀÌ, ÀÇ */
};
int i;

	for (i=0; i<7; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHMID(hch,HKEYTOMID(table[i][2])));
	return 0;
}

static HANCHAR HIAMakeDLST2(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[11][3] = {
	{0x82, 0x8b, 0xc4},       /* ±â¿ª, ½Ã¿Ê */
	{0x84, 0x8e, 0xc6},       /* ´ÏÀº, ÁöÀÁ */
	{0x84, 0x94, 0xc7},       /* ´ÏÀº, È÷ÀÆ */
	{0x87, 0x82, 0xca},       /* ¸®À», ±â¿ª */
	{0x87, 0x88, 0xcb},       /* ¸®À», ¹ÌÀ½ */
	{0x87, 0x89, 0xcc},       /* ¸®À», ºñÀ¾ */
	{0x87, 0x8b, 0xcd},       /* ¸®À», ½Ã¿Ê */
	{0x87, 0x92, 0xce},       /* ¸®À», Æ¼ÀÄ */
	{0x87, 0x93, 0xcf},       /* ¸®À», ÇÇÀÅ */
	{0x87, 0x94, 0xd0},       /* ¸®À», È÷ÀÆ */
	{0x89, 0x8b, 0xd4}		  /* ºñÀ¾, ½Ã¿Ê */
};
int i;

	for (i=0; i<11; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHLST(hch,table[i][2]-0xc0));
	return 0;
}

HANCHAR HIAAutomata2(HIA *phia,USHORT key)
{

HIABuf		*inbuf;
HANCHAR     hch = 0;
HANCHAR     result = 0;
HANCHAR     oldhch;
USHORT		oldkey;


	inbuf = (phia->inbuf);
	oldhch = HIABufPeekHch(inbuf);
	oldkey = HIABufPeekKey(inbuf);

	switch (inbuf->newpos)
		{
		case HIABUF_NONE:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
				HIABufPush(inbuf,hch,key);

				} else {

				HIABufPush(inbuf,0,0);	// NULL for FST
				HIABufPush(inbuf,0,0);	// NULL for DFST

				hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);
				}
			break;
		case HIABUF_FST:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				if (hch = HIAMakeDFST2(oldhch,oldkey,key))
					{
					HIABufPush(inbuf,hch,key);
					} else {
					result = HIABufPopHch(inbuf);
					HIABufClear(inbuf);
					hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
					HIABufPush(inbuf,hch,key);
					}
				} else {
				HIABufPush(inbuf,0,0);	// NULL for DFST
				hch = HCHWITHMID(oldhch,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);
				}
			break;
		case HIABUF_DFST:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);
				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
				HIABufPush(inbuf,hch,key);
				} else {
				hch = HCHWITHFST(oldhch,HKEYTOFST(key));
				HIABufPush(inbuf,hch,key);
				}
			break;
		case HIABUF_MID:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				oldhch = HIABufPeekHch(inbuf);

				HIABufPush(inbuf,0,0);	// NULL for DMID

				if (HKEYCANBELST(key))
					{

					hch = HCHWITHLST(oldhch,HKEYTOLST(key));
					HIABufPush(inbuf,hch,key);

					} else {

					result = oldhch;
					HIABufClear(inbuf);

					hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
					HIABufPush(inbuf,hch,key);

					}


				} else { // HIA_KEY_VOWEL

				if ((hch = HIAMakeDMID2(oldhch,oldkey,key)) != 0)
					{
					HIABufPush(inbuf,hch,key);	// hch for DMID

					} else {

					result = HIABufPopHch(inbuf);
					HIABufClear(inbuf);

					HIABufPush(inbuf,0,0);	// NULL for FST
					HIABufPush(inbuf,0,0);	// NULL for DFST

					hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
					HIABufPush(inbuf,hch,key);

					}
				}
			break;
		case HIABUF_DMID:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{

				if (HKEYCANBELST(key))
					{
					hch = HCHWITHLST(oldhch,HKEYTOLST(key));
					HIABufPush(inbuf,hch,key);
					} else {
					result = oldhch;
					HIABufClear(inbuf);

					hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
					HIABufPush(inbuf,hch,key);
					}

				} else {
				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);

				HIABufPush(inbuf,0,0);	// NULL for FST
				HIABufPush(inbuf,0,0);	// NULL for DFST

				hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);
				}
			break;
		case HIABUF_LST:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				if (hch = HIAMakeDLST2(oldhch,oldkey,key))
					{
					HIABufPush(inbuf,hch,key);

					} else {

					result = HIABufPopHch(inbuf);
					HIABufClear(inbuf);

					hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
					HIABufPush(inbuf,hch,key);	// hch for FST

					}
				} else {
				oldkey = HIABufPopKey(inbuf);

				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);

				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(oldkey));
				HIABufPush(inbuf,hch,oldkey);	// hch for FST
				HIABufPush(inbuf,0,0);			// NULL for DFST

				hch = HCHWITHMID(hch,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);		// hch for MID

				}
			break;
		case HIABUF_DLST:
			if (HKEYKIND(key) == HIA_KEY_CONSONANT)
				{
				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);

				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
				HIABufPush(inbuf,hch,key);

				} else {

				oldkey = HIABufPopKey(inbuf);

				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);

				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(oldkey));
				HIABufPush(inbuf,hch,oldkey);	// hch for FST

				HIABufPush(inbuf,0,0);			// null for DFST

				hch = HCHWITHMID(hch,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);		// hch for MID

				}
			break;
		} /* switch */

	return result;

}
