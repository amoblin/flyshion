SUBDIRS = src include skin resource

EXTRA_DIST = LICENSE LICENSE.OpenSSL 

install: install-recursive
	/usr/bin/ginstall -c -m 644 skin/fetion.jpg /usr/local/share/icons/fetion.jpg
	/usr/bin/ginstall -c -m 644 resource/openfetion.desktop /usr/share/applications
	@if [ -h /usr/bin/openfetion ] ;then \
		rm /usr/bin/openfetion; \
	fi
	ln -s $(prefix)/bin/openfetion /usr/bin/openfetion
	@if [ -d ~/.openfetion ];\
	then \
		cd ~/.openfetion; \
		rm global.dat 2>/dev/null; \
		find . -name "config.dat" -exec rm {} \;;\
	fi
	@echo $(pkgdatadir)/skin/
	@echo "----------------------------------------------------------";
	@echo "|                                                        |";
	@echo "|       OpenFetion 1.6 by lwp(levin108@gmail.com)        |";
	@echo "|                                                        |";
	@echo "|  OpenFetion a fetion client for linux based on GTK+2.0,|";
	@echo "|  using Fetion Protocol Version 4. It supports most     |";
	@echo "|  useful functions of Mobile Fetion,more important.     |";
	@echo "|  It\`s small and fast,and is better in look.            |";
	@echo "|  More information at http://basiccoder.com/openfetion  |";
	@echo "----------------------------------------------------------";
