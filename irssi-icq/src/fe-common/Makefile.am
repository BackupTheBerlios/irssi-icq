moduledir = $(libdir)/irssi/modules

module_LTLIBRARIES = libfe_icq.la
libfe_icq_la_LDFLAGS = -avoid-version

INCLUDES = \
	$(GLIB_CFLAGS) \
	-I $(top_srcdir)/src/gaimglue \
	-I$(IRSSI_INCLUDE) -I$(IRSSI_INCLUDE)/src \
	-I$(IRSSI_INCLUDE)/src/core -I$(IRSSI_INCLUDE)/src/fe-common/core \
	-I$(top_srcdir)/src/core

libfe_icq_la_SOURCES = \
	fe-icq.c \
	module-formats.c

noinst_HEADERS = \
	module.h \
	module-formats.h











