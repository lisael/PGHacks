TESTS = check_pghx
check_PROGRAMS = check_pghx
check_pghx_SOURCES = check_pghx.c $(top_builddir)/src/pghx/logicaldecoding.h
check_pghx_CFLAGS = @CHECK_CFLAGS@ -lsubunit -lpq
check_pghx_LDADD = $(top_builddir)/src/pghx/libpghx.la @CHECK_LIBS@
AM_CPPFLAGS = -Wdate-time -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -I/usr/include -I`pg_config --includedir` -I`pg_config --includedir-server` -I/usr/include/mit-krb5
AM_CFLAGS = -Wall -Wpointer-arith -Wdeclaration-after-statement -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -g -g -O2 -fstack-protector-strong -Wformat -Werror=format-security -fPIC -pie -fno-omit-frame-pointer
