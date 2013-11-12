SUBDIRS = src test/unit
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure depcomp install-sh \
                       missing src/Makefile.in

EXTRA_DIST = test doc
dist_man1_MANS = doc/pk.1

test: src/pk
	test/system/run
