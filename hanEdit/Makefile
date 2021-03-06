.SUFFIXES:	.c .o .obj .rc .res
CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s

CP = cp
CP2 = copy
RM = rm
RC = rc

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@
.rc.res:
	rc -r $*.rc

HAN_DIR = ../hanlib
HAN_A = ../hanlib/han.a
HAN_LIB = ../hanlib/han.lib
HAN_H = \
	$(HAN_DIR)/han.h \
	$(HAN_DIR)/hch.h\
	$(HAN_DIR)/hch/hch.h\
	$(HAN_DIR)/hout.h \
	$(HAN_DIR)/hin.h

ETC_DIR = ../etclib
ETC_H = \
	$(ETC_DIR)/textutil.h\
	$(ETC_DIR)/debugutil.h
ETC_O = $(ETC_H:.h=.o)
ETC_OBJ = $(ETC_H:.h=.obj)

HCT_DIR = ../hct
HCT_H = \
	$(HCT_DIR)/hct.h\
	$(HCT_DIR)/ktype.h
HCT_O = $(HCT_DIR)/hct.o
HCT_OBJ = $(HCT_DIR)/hct.obj

HCHLB_DIR = ..\hchlb
HCHLB_H = $(HCHLB_DIR)\hchlbdlg.h
HCHLBDLG_RES = $(HCHLB_DIR)\hchlbdlg.res

HMLE_DIR = ../HMLE
HMLE_LIB = ../HMLE/HMLE.lib
HMLE_A = ../HMLE/HMLE.a
HMLE_H = ../HMLE/HMLE.h

HST_DIR = ../HST
HST_LIB = ../HST/HST.lib
HST_A = ../HST/HST.a
HST_H = ../HST/HST.h

HEF_DIR = ../HEF
HEF_LIB = ../HEF/HEF.lib
HEF_A = ../HEF/HEF.a
HEF_H = ../HEF/HEF.h

#target
debug:	HMLEdit.exe
release: hanedit.exe
all:	HMLEdit.exe hanedit.exe

LIBS_H = $(HMLE_H) $(HST_H) $(HEF_H) $(HAN_H) $(ETC_H) $(HCT_H)

#hanedit Develop version
HMLEDIT_O = HMLEdit.o statbar.o toolbar.o dlg.o
HMLEdit.exe:	$(HMLEDIT_O) $(HMLE_A) $(HST_A) $(HEF_A) $(HAN_A) $(ETC_O) $(HCT_O) HMLEdit.def HMLEdit.res
	$(CC) $(CFLAGS) -o HMLEdit.exe $(HMLEDIT_O) $(HMLE_A) $(HST_A) $(HEF_A) $(HAN_A) $(ETC_O) $(HCT_O) HMLEdit.def HMLEdit.res -Zstack 0x2000

HMLEdit.o:	HMLEdit.h res.h $(LIBS_H)
statbar.o:	statbar.h $(LIBS_H)
toolbar.o:	toolbar.h $(LIBS_H)
dlg.o:		dlg.h $(LIBS_H)

$(HCHLBDLG_RES) :
	$(MAKE) -C $(HCHLB_DIR) hchlbdlg.res

HMLEdit.res:	HMLEdit.rc res.h dlg.dlg $(HCHLBDLG_RES) $(HMLE_H) $(HST_H) $(HEF_H)
	$(RC) -r hmledit.rc
	$(CP2) /b hmledit.res+$(HCHLBDLG_RES) hmledit.res

#hanedit Release version
HMLEDIT_OBJ = $(HMLEDIT_O:.o=.obj)
hanedit.exe:	$(HMLEDIT_OBJ) $(HMLE_LIB) $(HST_LIB) $(HEF_LIB) $(HAN_LIB) $(ETC_OBJ) $(HCT_OBJ) HMLEdit.def HMLEdit.res
	$(CC) $(CFLAGS2) -o hanedit.exe $(HMLEDIT_OBJ) $(HMLE_LIB) $(HST_LIB) $(HEF_LIB) $(HAN_LIB) $(ETC_OBJ) $(HCT_OBJ) hanedit.def HMLEdit.res -Zstack 0x2000

HMLEdit.obj:	HMLEdit.h $(LIBS_H)
statbar.obj:	statbar.h $(LIBS_H)
toolbar.obj:	toolbar.h $(LIBS_H)
dlg.obj:	dlg.h $(LIBS_H)

touch:
	touch *.c *.h *.rc

clean:
	-$(RM) *.o *.obj *.a *.lib *.res *.exe *.bak *.rwp core *~*
