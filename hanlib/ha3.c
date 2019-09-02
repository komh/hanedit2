#include "hin.h"
#include "hin_internal.h"

#define HIA_KEY_FST		0x00
#define HIA_KEY_MID		0x20
#define HIA_KEY_LST		0x40
#define HKEYKIND(a)		((a) & 0x60)

#define HKEYTOFST(key)		(((key)-0x80))	//<<10)
#define HKEYTOMID(key)		(((key)-0xA0))	//<<5)
#define HKEYTOLST(key)		(((key)-0xC0))

static HANCHAR HIAMakeDFST3(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[5][3] = {
	{0x82, 0x82, 0x03},		/* ‹¡µb, ‹¡µb, °w‹¡µb */
	{0x85, 0x85, 0x06},		/* —¡‹h, —¡‹h, °w—¡‹h */
	{0x89, 0x89, 0x0a},		/* §¡·s, §¡·s, °w§¡·s */
	{0x8b, 0x8b, 0x0c},		/* ¯¡µµ, ¯¡µµ, °w¯¡µµ */
	{0x8e, 0x8e, 0x0f}		/* »¡·x, »¡·x, °w»¡·x */
};
int i;

	for (i=0; i<5; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHFST(hch,table[i][2]));

return 0;
}

static HANCHAR HIAMakeDMID3(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[7][3] = {
	{0xad, 0xa3, 0x0e},       /* ¿À, ¾Æ, ¿Í */
	{0xad, 0xa4, 0x0f},       /* ¿À, ¾Ö, ¿Ö */
	{0xad, 0xbd, 0x12},       /* ¿À, ÀÌ, ¿Ü */
	{0xb4, 0xa7, 0x15},       /* ¿ì, ¾î, ¿ö */
	{0xb4, 0xaa, 0x16},       /* ¿ì, ¿¡, ¿þ */
	{0xb4, 0xbd, 0x17},       /* ¿ì, ÀÌ, À§ */
	{0xbb, 0xbd, 0x1c}		  /* À¸, ÀÌ, ÀÇ */
	};
int i;

	for (i=0; i<7; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHMID(hch,table[i][2]));
return 0;
}

static HANCHAR HIAMakeDLST3(HANCHAR hch,int oldkey,int key)
{
static unsigned char table[11][3] = {
	{0xC2, 0xD5, 0x04},		/* Gi , Si */
	{0xC5, 0xD8, 0x06},		/* Ni , Ji */
	{0xC5, 0xDD, 0x07},		/* Ni , Hi */
	{0xC9, 0xC2, 0x0A},		/* Ri , Gi */
	{0xC9, 0xD1, 0x0B},		/* Ri , Mi */
	{0xC9, 0xD3, 0x0C},		/* Ri , Bi */
	{0xC9, 0xD5, 0x0D},		/* Ri , Si */
	{0xC9, 0xDB, 0x0E},		/* Ri , Ti */
	{0xC9, 0xDC, 0x0F},		/* Ri , Pi */
	{0xC9, 0xDD, 0x10},		/* Ri , Hi */
	{0xD3, 0xD5, 0x14}		/* Bi , Si */
};
int i;

	for (i=0; i<11; i++)
		if (table[i][0] == oldkey && table[i][1] == key)
			return (HCHWITHLST(hch,table[i][2]));
return 0;
}

HANCHAR HIAAutomata3(HIA *phia,USHORT key)
{

HIABuf		*inbuf;
HANCHAR     hch;
HANCHAR     result = 0;
HANCHAR     oldhch;
USHORT		oldkey;


	inbuf = (phia->inbuf);
	oldhch = HIABufPeekHch(inbuf);
	oldkey = HIABufPeekKey(inbuf);

	switch (inbuf->newpos) {
	case HIABUF_NONE:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST

			hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key); // for MID
			break;
		case HIA_KEY_LST:
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST
			HIABufPush(inbuf,0,0);	// null for MID
			HIABufPush(inbuf,0,0);	// null for DMID

			hch = HCHWITHLST(HCH_BASECODE,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key); // for LST
			break;
		}
		break;
	case HIABUF_FST:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			if ((hch = HIAMakeDFST3(oldhch,oldkey,key)) != 0) // if can make fst pair
				{
				HIABufPush(inbuf,hch,key);	// for DFST
				} else {
				result = HIABufPopHch(inbuf);	// complete current hch
				HIABufClear(inbuf);

				hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
				HIABufPush(inbuf,hch,key);	// for FST
				}
			break;
		case HIA_KEY_MID:
			HIABufPush(inbuf,0,0);	// null for DFST

			hch = HCHWITHMID(oldhch,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key); // for MID
			break;
		case HIA_KEY_LST:
			HIABufPush(inbuf,0,0);	// null for DFST
			HIABufPush(inbuf,0,0);	// null for MID

			hch = HCHWITHLST(oldhch,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key); // for LST
			break;
		}
		break;
	case HIABUF_DFST:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);

			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			hch = HCHWITHMID(oldhch,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_LST:
			hch = HCHWITHLST(oldhch,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key);
			break;
		}
		break;
	case HIABUF_MID:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			result = HIABufPopHch(inbuf);	// complete current hch
			HIABufClear(inbuf);			

			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_LST:
			HIABufPush(inbuf,0,0);	// null for DMID

			hch = HCHWITHLST(oldhch,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			if ((hch = HIAMakeDMID3(oldhch,oldkey,key)) != 0)
				{
				HIABufPush(inbuf,hch,key);	// DMID
				} else {
				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);
				HIABufPush(inbuf,0,0);	// null for FST
				HIABufPush(inbuf,0,0);	// null for DFST

				hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
				HIABufPush(inbuf,hch,key);// hch for MID
				}
			break;
		}
		break;
	case HIABUF_DMID:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);

			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);
			
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST

			hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key); // hch for MID
			break;
		case HIA_KEY_LST:
			hch = HCHWITHLST(oldhch,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key);
			break;
		}
		break;
	case HIABUF_LST:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);

			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);
			
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST

			hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_LST:
			if (hch = HIAMakeDLST3(oldhch,oldkey,key))
				{
				HIABufPush(inbuf,hch,key);
				} else {
				result = HIABufPopHch(inbuf);
				HIABufClear(inbuf);
				
				HIABufPush(inbuf,0,0);	// null for FST
				HIABufPush(inbuf,0,0);	// null for DFST
				HIABufPush(inbuf,0,0);	// null for MID
				HIABufPush(inbuf,0,0);	// null for DMID

				hch = HCHWITHLST(HCH_BASECODE,HKEYTOLST(key));
				HIABufPush(inbuf,hch,key);
				}
			break;
		}
		break;
	case HIABUF_DLST:
		switch (HKEYKIND(key)) {
		case HIA_KEY_FST:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);
			
			hch = HCHWITHFST(HCH_BASECODE,HKEYTOFST(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_MID:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST

			hch = HCHWITHMID(HCH_BASECODE,HKEYTOMID(key));
			HIABufPush(inbuf,hch,key);
			break;
		case HIA_KEY_LST:
			result = HIABufPopHch(inbuf);
			HIABufClear(inbuf);
			HIABufPush(inbuf,0,0);	// null for FST
			HIABufPush(inbuf,0,0);	// null for DFST
			HIABufPush(inbuf,0,0);	// null for MID
			HIABufPush(inbuf,0,0);	// null for DMID

			hch = HCHWITHLST(HCH_BASECODE,HKEYTOLST(key));
			HIABufPush(inbuf,hch,key);
			break;
		}
		break;
	} /* switch */

	return result;

}
