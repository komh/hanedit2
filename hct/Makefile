.SUFFIXES:	.o .c .obj
CC = gcc
CFLAGS = -Wall -Wno-format -Wno-parentheses -O -DDEBUG -g
CFLAGS2 = -Zomf -Zsys -Wall -Wno-format -Wno-parentheses -O3 -s
RM = rm
CP = cp

.c.o:
	$(CC) $*.c $(CFLAGS) -c -o $@
.c.obj:
	$(CC) $*.c $(CFLAGS2) -c -o $@

ALL_O = hct.o
ALL_OBJ = $(ALL_O:.o=.obj)

#targets:
debug: $(ALL_O)
release: $(ALL_OBJ)
all:	$(ALL_O) $(ALL_OBJ)
debug_hanedit: hct.o
release_hanedit: hct.obj

# O
hct.o: hct.h ktype.h
hct.obj: hct.h ktype.h

new:
	touch *.c *.h
	$(MAKE) all

#clear

clean:
	-$(RM) *.o *.obj *.bak core *~
