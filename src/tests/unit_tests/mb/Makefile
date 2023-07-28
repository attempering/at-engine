sub_dirs = iie io

bins::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

clean::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;
