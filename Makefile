zip::
	zip -r --symlinks ../at-engine_`date +'%Y-%m-%d-%H%M'`.zip \
	    --exclude="*.zip" \
	    --exclude="*.vscode*" \
	    --exclude="*.git*" \
	    *

clean::
	$(MAKE) $@ -C src

countlines::
	find . -name "*.[ch]" | xargs wc
