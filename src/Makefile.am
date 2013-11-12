AM_CFLAGS = -std=c99

bin_PROGRAMS = pk

pk_SOURCES = compat.c command_line.c configuration.c fieldprinter.c helpers.c \
             main.c stringlist.c tokenizer.c wrappers.c

pk_LDADD = $(ARGP_LIBS)

noinst_HEADERS = compat.h command_line.h configuration.h fieldprinter.h \
                 helpers.h stringlist.h tokenizer.h wrappers.h
