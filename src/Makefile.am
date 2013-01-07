AM_CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -D_BSD_SOURCE -D_GNU_SOURCE \
            -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align \
            -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 \
            -Wuninitialized -Wmissing-declarations -Wno-long-long \
            -Wno-unused-parameter -Wno-missing-field-initializers

bin_PROGRAMS = fieldx

fieldx_SOURCES = main.c configuration.c fieldprinter.c stringlist.c \
                 tokenizer.c wrappers.c

noinst_HEADERS = configuration.h fieldprinter.h stringlist.h tokenizer.h \
                 wrappers.h
