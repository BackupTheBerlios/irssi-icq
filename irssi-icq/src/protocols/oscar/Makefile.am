EXTRA_DIST = \
		README \
		CHANGES \
		COPYING \
		BUGS \
		AUTHORS \
		Makefile.mingw

pkgdir = $(libdir)/gaim

OSCARSOURCES = \
	admin.c        \
	adverts.c      \
	aim.h          \
	aim_cbtypes.h  \
	aim_internal.h \
	auth.c         \
	bart.c         \
	bos.c          \
	bstream.c      \
	buddylist.c    \
	chat.c         \
	chatnav.c      \
	conn.c         \
	email.c        \
	faimconfig.h   \
	ft.c           \
	icq.c          \
	im.c           \
	info.c         \
	invite.c       \
	md5.h          \
	misc.c         \
	msgcookie.c    \
	odir.c         \
	popups.c       \
	rxhandlers.c   \
	rxqueue.c      \
	search.c       \
	service.c      \
	snac.c         \
	ssi.c          \
	stats.c        \
	tlv.c          \
	translate.c    \
	txqueue.c      \
	util.c         \
	oscar.c

AM_CFLAGS = $(st)

#liboscar_la_LDFLAGS = -module -avoid-version

#dnl if STATIC_OSCAR

st = -DSTATIC
noinst_LIBRARIES   = liboscar.a
liboscar_a_SOURCES = $(OSCARSOURCES)
liboscar_a_CFLAGS  = $(AM_CFLAGS)

#dnl else

#dnl st =
#dnl pkg_LTLIBRARIES     = liboscar.la
#dnl liboscar_la_SOURCES = $(OSCARSOURCES)

#dnl endif
#dnl do it static.


AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLIB_CFLAGS) \
	$(DEBUG_CFLAGS)


#dnl 	$(GTK_CFLAGS) 
