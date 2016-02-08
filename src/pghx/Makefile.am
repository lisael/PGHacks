lib_LTLIBRARIES = libpghx.la
libpghx_la_SOURCES = errors.c logicaldecoding.c utils.c ldtestparser.c dump/pghx_dump.c
pkginclude_HEADERS = utils.h errors.h logicaldecoding.h ldtestparser.h dump/pghx_dump.h
AM_CPPFLAGS = -Wdate-time -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -I`pg_config --includedir` -I`pg_config --includedir-server` -I/usr/include/mit-krb5
AM_CFLAGS = -Wall -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -fPIC -pie -fno-omit-frame-pointer
