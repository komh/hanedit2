.SUFFIXES:	.c .o .obj
CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s

CP = cp
RM = rm

HAN_DIR = ../hanlib
HAN_A = $(HAN_DIR)/han.a
HAN_LIB = $(HAN_DIR)/han.lib
HAN_H = \
	$(HAN_DIR)/han.h\
	$(HAN_DIR)/hch.h\
	$(HAN_DIR)/hch/hch.h\
	$(HAN_DIR)/hout.h\
	$(HAN_DIR)/hin.h

ETC_DIR = ../etclib
ETC_H = \
	$(ETC_DIR)/textutil.h\
	$(ETC_DIR)/debugutil.h
ETC_O = $(ETC_H:.h=.o)
ETC_OBJ = $(ETC_H:.h=.obj)

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@

#all
debug:	HST.a
release:	HST.lib
all:	HST.a HST.lib

HST_O = HST.o
HST_OBJ = $(HST_O:.o=.obj)

HST.a:	$(HST_O)
	ar cvr HST.a $?
HST.lib:	$(HST_OBJ)
	emxomfar cvr HST.lib $?

HST.o:		HST.h $(HAN_H)
HST.obj:	HST.h $(HAN_H)

touch:
	touch *.c *.h

clean:
	-$(RM) *.o *.obj *.a *.lib *.bak core *~

