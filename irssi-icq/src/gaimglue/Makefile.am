
INCLUDES = \
	$(GLIB_CFLAGS) \
	-I$(IRSSI_INCLUDE) -I$(IRSSI_INCLUDE)/src \
	-I$(IRSSI_INCLUDE)/src/core \
	-I $(top_srcdir)/src/core 

pkgdir = $(libdir)/gaim

GAIMGLUESOURCES = \
	privacy.c \
	prpl.c \
	html.c \
	util.c \
	blist.c \
	proxy.c \
	conversation.c \
	server.c \
	ft.c \
	md5.c \
	account.c \
	plugin.c \
	value.c \
	signals.c \
	notify.c \
	request.c \
	prefs.c \
	imgstore.c \
	connection.c \
	log.c \
	buddyicon.c \
	accountopt.c


#	debug.c \
#	gaim_glue.c \



AM_CFLAGS = $(st)

st = -DSTATIC
noinst_LIBRARIES   = libgaimglue.a
libgaimglue_a_SOURCES = $(GAIMGLUESOURCES)
libgaimglue_a_CFLAGS  = $(AM_CFLAGS)
