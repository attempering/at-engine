bins::
	$(MAKE) $@ -C basic_tests
	$(MAKE) $@ -C unit_tests
	$(MAKE) $@ -C sample_progs

clean::
	$(MAKE) $@ -C basic_tests
	$(MAKE) $@ -C unit_tests
	$(MAKE) $@ -C sample_progs

zip::
	zip -r ../at-engine-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
