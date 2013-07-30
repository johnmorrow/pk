SUBDIRS = src
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure depcomp install-sh \
                       missing src/Makefile.in

EXTRA_DIST = test

test: src/pk
	test/system/run
