CP = cp

default:	hanedit.exe

hanedit.exe:
	$(MAKE) -C etclib release_hanedit
	$(MAKE) -C hanlib release
	$(MAKE) -C HMLE release
	$(MAKE) -C HEF release
	$(MAKE) -C HST release
	$(MAKE) -C hanEdit release
	$(CP) hanEdit/hanedit.exe .

clean:
	$(MAKE) -C etclib clean
	$(MAKE) -C hanlib clean
	$(MAKE) -C HMLE clean
	$(MAKE) -C HEF clean
	$(MAKE) -C HST clean
	$(MAKE) -C hanEdit clean
