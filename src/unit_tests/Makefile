bins::
	$(MAKE) $@ -C at
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C langevin

clean::
	$(MAKE) $@ -C at
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C langevin

zip::
	zip -r ../at-engine-unit-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *
