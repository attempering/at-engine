sub_dirs = basic_tests unit_tests sample_progs

bins::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

clean::
	for sub_dir in $(sub_dirs); do \
		$(MAKE) $@ -C $$sub_dir; \
	done;

zip::
	zip -r ../at-engine-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
