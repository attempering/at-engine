zip::
	zip -r ../at-engine_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    *

clean::
	$(MAKE) $@ -C src
