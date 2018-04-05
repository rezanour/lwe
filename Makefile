TOPTARGETS := all clean

SUBDIRS := lwe_common/src lwe_graphics/src lwe_test_app1

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)