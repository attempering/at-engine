bins::
	$(MAKE) $@ -C at
	$(MAKE) $@ -C distr
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C driver

clean::
	$(MAKE) $@ -C at
	$(MAKE) $@ -C distr
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C driver

zip::
	zip -r ../at-engine-unit-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
