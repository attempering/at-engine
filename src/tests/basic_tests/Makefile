sub_dirs = zcom at distr mb driver

bins::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

clean::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

zip::
	zip -r ../at-engine-basic-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
