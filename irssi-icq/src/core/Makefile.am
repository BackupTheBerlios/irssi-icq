moduledir = $(libdir)/irssi/modules

module_LTLIBRARIES = libicq_core.la
libicq_core_la_LDFLAGS = -avoid-version

libicq_core_la_LIBADD=$(top_srcdir)/src/protocols/oscar/liboscar.a $(top_srcdir)/src/gaimglue/libgaimglue.a

INCLUDES = \
	-I $(top_srcdir)/src/gaimglue \
	-I $(top_srcdir)/src/protocols/oscar/ \
	$(GLIB_CFLAGS) \
	-I$(IRSSI_INCLUDE) -I$(IRSSI_INCLUDE)/src \
	-I$(IRSSI_INCLUDE)/src/core 


libicq_core_la_SOURCES = \
	icq-commands.c \
	icq-core.c \
	icq-queries.c \
	icq-servers-reconnect.c \
	icq-protocol.c \
	icq-servers.c \
	icq-buddy.c \
	icq-session.c 

noinst_HEADERS = \
	icq.h \
	icq-commands.h \
	icq-protocol.h \
	icq-queries.h \
	icq-servers.h \
	icq-buddy.h \
	module.h

user_install:
	cp -f .libs/libicq_core.so $(HOME)/.irssi/modules/













