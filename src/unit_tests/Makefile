zip::
	zip -r ../at-engine-unit-tests_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *

clean::
	$(MAKE) $@ -C mb
	$(MAKE) $@ -C langevin
