SHELL = $(COMSPEC)
CP = cp

default: debug

release:
	$(MAKE) -C hct release_hanedit
	$(MAKE) -C etclib release_hanedit
	$(MAKE) -C hanlib release
	$(MAKE) -C hchlb release
	$(MAKE) -C HMLE release
	$(MAKE) -C HEF release
	$(MAKE) -C HST release
	$(MAKE) -C hanEdit release
	$(CP) hanEdit/hanedit.exe .

debug:
	$(MAKE) -C hct debug_hanedit
	$(MAKE) -C etclib debug_hanedit
	$(MAKE) -C hanlib debug
	$(MAKE) -C hchlb debug
	$(MAKE) -C HMLE debug
	$(MAKE) -C HEF debug
	$(MAKE) -C HST debug
	$(MAKE) -C hanEdit debug
	$(CP) hanEdit/hmledit.exe .

clean:
	$(MAKE) -C hct clean
	$(MAKE) -C etclib clean
	$(MAKE) -C hanlib clean
	$(MAKE) -C hchlb clean
	$(MAKE) -C HMLE clean
	$(MAKE) -C HEF clean
	$(MAKE) -C HST clean
	$(MAKE) -C hanEdit clean
