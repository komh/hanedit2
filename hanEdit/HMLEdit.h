#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <assert.h>
#include "../etclib/debugutil.h"
#endif

#include "../hanlib/han.h"
#include "../HMLE/HMLE.h"
#include "../HST/HST.h"
#include "../HEF/HEF.h"

#include "res.h"

// ------------------------------------------------

#define HEWM_INIT		(WM_USER)
#define HEWM_CALCRECT	(WM_USER+1)

#define TOOLBAR_TITLE		0
#define TOOLBAR_OPEN		1
#define TOOLBAR_SAVE		2
#define TOOLBAR_RELOAD		3
#define TOOLBAR_EXIT		4
#define TOOLBAR_BLANK		5
#define TOOLBAR_COPY		6
#define TOOLBAR_CUT			7
#define TOOLBAR_PASTE		8
#define TOOLBAR_CLEAR		9

