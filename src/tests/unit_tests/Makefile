bins::
	$(MAKE) $@ -C distr
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C driver
	$(MAKE) $@ -C at

clean::
	$(MAKE) $@ -C distr
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C driver
	$(MAKE) $@ -C at

zip::
	zip -r ../at-engine-unit-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
