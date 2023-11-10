sub_dirs = e_fixed gaussian double_well lj

bins::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

clean::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;