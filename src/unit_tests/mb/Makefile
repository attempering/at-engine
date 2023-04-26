zip::
	zip -r ../pcst-core-unit-tests-mb_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    *

clean::
	$(MAKE) $@ -C zerofiller
	$(MAKE) $@ -C integrator
	$(MAKE) $@ -C iie
