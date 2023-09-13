sub_dirs = simple log

bins::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

clean::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;
