/*
 icq-protocol.c : irssi

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "module.h"
#include "signals.h"
#include "network.h"
#include "net-sendbuffer.h"
#include "rawlog.h"
#include "icq-protocol.h"

#include "icq-servers.h"
#include "icq-queries.h"
#include "icq-buddy.h"

#include "fe-common/core/printtext.h"
#include "levels.h"
#include "irssi-version.h"

//#define DEBUG(msg, param...) fprintf(stderr, msg "\n" , ## param)
//#define DEBUG(msg, param...) printtext(NULL, NULL, MSGLEVEL_CLIENTNOTICE, msg ,##param)
//#define NOTICE(msg, param...) printtext(sock, NULL, MSGLEVEL_CLIENTNOTICE, msg ,##param)
#define DEBUG(...)
#define NOTICE(...)

#define ERROR(msg, param...) printtext(NULL, NULL, MSGLEVEL_CLIENTERROR, msg ": %s (%d)" , ## param , strerror(errno), errno)

#define OVERHEAD 70 /* the maximum packet encapsulation overhead */

static char *encode_passwd(char *buf, const char *pwd);

#define MAX_UIN 45

static int my_status = 0;

#define writeb(buf, value) (*(buf) = (value), (buf)+1)
static inline char *writew(char *buf, guint16 value)
	{ *((guint16*)buf)++ = htons(value); return buf; }
static inline char *writel(char *buf, guint32 value)
	{ *((guint32*)buf)++ = htonl(value); return buf; }
static inline char *writes(char *buf, const char *data, int len)
	{ memcpy(buf,data,len); return buf+len; }

static char *tlv_begin(char *buf, guint16 value) {
	buf = writew(buf, value);
	return buf+2;
}
static char *tlv_end(char *buf, char *start) {
	start -= 2;
	writew(start, buf-start-2);
	return buf;
}
static char *write_tlv(char *buf, int type, const char *data, int len) {
	buf = writew(buf, type);
	buf = writew(buf, len);
	return writes(buf, data, len);
}

#define readb(buf) (*((guint8*)buf)++)
#define readw(buf) __extension__(({ guint16 ret = ntohs(*(guint16*)buf); ((guint16*)buf)++; ret; }))
#define readl(buf) __extension__(({ guint32 ret = ntohl(*(guint32*)buf); ((guint32*)buf)++; ret; }))

static char *read_tlv(char *buf, char *end, int *type, char **data, int *len)
{
	if (end - buf < 4) return NULL;
	*type = readw(buf);
	*len = readw(buf);
	if (end - buf < *len) return NULL;
	*data = buf;
	return buf + *len;
}

static char *scan_tlv(char *buf, char *end, int type, int *len)
{
	char *data;
	int typ;
	do {
		buf = read_tlv(buf, end, &typ, &data, len);
		DEBUG("tlv type %d len %d", typ, *len);
		if (!buf) return NULL;
	} while (type != typ);
	return data;
}

static int process_packet(ICQ_SERVER_REC *sock, char *buf, char *end, int channel);

static char *decode_flap(ICQ_SERVER_REC *sock, char *buf, char *end)
{
	int seq, data_len;
	int magic, channel;
	char *start = buf;

	if (end-start < 6)
		return start;
	magic = readb(buf);
	if (magic != 0x2A) {
		NOTICE("wrong magic number in packet!");
		return NULL;
	}
	channel = readb(buf);
	seq = readw(buf);
	data_len = readw(buf);
	NOTICE("Incoming packet. chan=%d seq=%d len=%d of=%d", channel, seq, data_len, end-buf);
	if (end - buf < data_len)
		return start;
	end = buf + data_len;
	process_packet(sock, buf, end, channel);
	return end;
}

static char *flap_begin(char *buf, char channel)
{
	static int seq = 0;

	buf = writeb(buf, 0x2A);
	buf = writeb(buf, channel);
	buf = writew(buf, ++seq);
	return buf+2;
}

static char *decode_snac(char *buf, char *end, int *type)
{
	int seq, flags;

	if (end - buf < 10) {
		return NULL;
	}

	*type = readl(buf);
	flags = readw(buf);
	seq = readl(buf);
	return buf;
}

static char *encode_snac(char *buf, int type)
{
	static int seq = 0;
	char *flap;
	buf = flap = flap_begin(buf, 2);
	buf = writel(buf, type);
	buf = writew(buf, 0);			/* flags */
	buf = writel(buf, ++seq);
	tlv_end(buf, flap);
	return buf;
}

static char *snac_end(char *buf, char *beg)
{
	return tlv_end(buf, beg-10);
}

static char *encode_msg(char *buf, const char *uin, const char *msg)
{
	int uin_len = strlen(uin);
	int msg_len = strlen(msg);
	char *tlv, *t;

	buf = writel(buf, 0);		/* random cookie */
	buf = writel(buf, 0);		/* random cookie hi */
	buf = writew(buf, 1);		/* channel */
	buf = writeb(buf, uin_len);
	buf = writes(buf, uin, uin_len);

	tlv = buf = tlv_begin(buf, 0x0002);
		t = buf = tlv_begin(buf, 0x0501);
			buf = writeb(buf, 0);
		tlv_end(buf, t);
		t = buf = tlv_begin(buf, 0x0101);
			buf = writel(buf, 0);
			buf = writes(buf, msg, msg_len);
		tlv_end(buf, t);
	tlv_end(buf, tlv);
	return buf;
}

#define STR(s) (s), (sizeof(s)-1)

static char *encode_login(char *buf, const char *uin, const char *passwd)
{
	int uin_len = strlen(uin);
	char *tlv, *flap;

	buf = flap = flap_begin(buf, 1);
	buf = writel(buf, 0x0000001);
	buf = write_tlv(buf, 0x01, uin, uin_len);
	buf = tlv = tlv_begin(buf, 0x02);
		buf = encode_passwd(buf, passwd);
	tlv_end(buf, tlv);
	buf = write_tlv(buf, 0x03, 
			STR(PACKAGE ", version " IRSSI_VERSION " (irssi icq plugin)"));
	buf = write_tlv(buf, 0x16, STR("\x01\x0a"));
	buf = write_tlv(buf, 0x17, STR("\x00\x04"));
	buf = write_tlv(buf, 0x18, STR("\x00\x3c"));
	buf = write_tlv(buf, 0x19, STR("\x00\x01"));
	buf = write_tlv(buf, 0x1A, STR("\x0c\x0e"));
	buf = write_tlv(buf, 0x14, STR("\x00\x00\x00\x55"));
	buf = write_tlv(buf, 0x0E, STR("us"));
	buf = write_tlv(buf, 0x0F, STR("en"));
	tlv_end(buf, flap);
	return buf;
}

static char *encode_reconnect(char *buf, char *cookie, int cookie_len)
{
	char *flap;

	buf = flap = flap_begin(buf, 1);
	buf = writel(buf, 0x0000001);
	buf = write_tlv(buf, 0x06, cookie, cookie_len);
	tlv_end(buf, flap);
	return buf;
}

static char *extract_user_info(char *buf, char *end, char *uin, int *ip, int *mode)
{
	int uin_len, warnlevel, tlvs;

	uin_len = readb(buf);
	if (uin_len >= MAX_UIN || end - buf <= uin_len+4 || uin_len <= 0) {
		DEBUG("Too long username! Increase UIN_LEN to %d letters.", uin_len);
		return NULL;
	}
	if (uin) {
		memcpy(uin, buf, uin_len);
		uin[uin_len] = '\0';
	}
	buf += uin_len;

	warnlevel = readw(buf);
	tlvs = readw(buf);

	while (tlvs--) {
		char *data;
		int type, len;

		buf = read_tlv(buf, end, &type, &data, &len);
		if (!buf) {
			DEBUG("partial read in tlvs");
			return NULL;
		}

		switch (type) {
		case 6:
			data += 2;
			if (mode) *mode = readw(data);
			break;
		case 10:
			if (ip) *ip = readl(data);
			break;
		}
	}
	return buf;
}

static char *decode_msg(char *buf, char *end, char *uin, int *ip)
{
	int cookie, channel;
	int len;

	if (end - buf < 16) {
		DEBUG("message packet too short!");
		return NULL;
	}
	cookie = readl(buf);
	cookie = readl(buf);
	channel = readw(buf);

	buf = extract_user_info(buf, end, uin, ip, NULL);
	if (!buf) return NULL;

	if (channel == 1) {
		buf = scan_tlv(buf, end, 2, &len);
		if (!buf) {
			DEBUG("type 2 not found in message");
			return NULL;
		}
		end = buf+len;

		buf = scan_tlv(buf, end, 0x0101, &len);
		if (!buf) {
			DEBUG("type 0101 not found in message");
			return NULL;
		}
		buf[len] = '\0';
		return buf+4;
	} else if (channel == 4) {
		buf = scan_tlv(buf, end, 5, &len);
		if (!buf) {
			DEBUG("type 4 tlv 5 not found in message");
			return NULL;
		}
		if (len < 8) {
			DEBUG("too short tlv 5 not found in message");
			return NULL;
		}
		*end = '\0';
		return buf + 8;
	} else {
		static char msg[] = "Unknown message type XXXXXXXXXXXX";
		sprintf(msg, "Unknown message type %d", channel);
		return msg;
	}
}

static char *encode_passwd(char *dest, const char *pwd)
{
	static const char xor_table[] = {
		0xf3, 0x26, 0x81, 0xc4,
		0x39, 0x86, 0xdb, 0x92,
		0x71, 0xa3, 0xb9, 0xe6,
		0x53, 0x7a, 0x95, 0x7c,
	};
	int i, len = strlen(pwd);

	for (i=0; i<len; i++) *dest++ = *pwd++ ^ xor_table[i%sizeof xor_table];
	return dest;
}

static int xwrite(ICQ_SERVER_REC *sock, char *buf, int len)
{
	if (net_sendbuffer_send(sock->handle, buf, len) == -1) {
		DEBUG("write failed");
		/* something bad happened */
		sock->connection_lost = TRUE;
		server_disconnect(SERVER(sock));
		return -1;
	}
	return 0;
}

static int xread(ICQ_SERVER_REC *sock, char *buf, int len) {
	int n;

	if (!IS_ICQ_SERVER(sock))
		return -1;

	if (!sock->handle) {
		DEBUG("no handle!");
		return -1;
	}
	n = net_receive(net_sendbuffer_handle(sock->handle), buf, len);
	if (n < 0) {
		//ERROR("net_receive");
		sock->connection_lost = TRUE;
		server_disconnect(SERVER(sock));
		return -1;
	}
	return n;
}

static void icq_parse_incoming(ICQ_SERVER_REC * sock)
{
	char *end;
	char * const buf = sock->inbuf;
	char *pos = sock->inbufp;
	int n = buf + SOCK_BUFSIZE - pos;
	/* buffer: |=previous data=|================================|
	 *         ^buf            ^ pos         n              end ^
	 */
	n = xread(sock, pos, n);
	if (n <= 0)
		return;
	end = pos + n;
	/* buffer: |=====valid=data================|================|
	 *         ^buf    ^pos->              end ^
	 */
	pos = buf;
	for (;;) {
		char *newpos = decode_flap(sock, pos, end);
		if (!newpos) {
			printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "Out of sync, discarding packet");
			sock->inbufp = buf;
			return;
		}
		if (newpos == end) {
			/* nice! nothing left. reset pointer */
			sock->inbufp = buf;
			return;
		}
		if (newpos == pos) {
			/* packet was not finished. Must recieve some more */
			n = end - pos;
			memmove(buf, pos, n);
			sock->inbufp = buf + n;
			return;
		}
		pos = newpos;
	}
}

static void icq_add_buddy(ICQ_SERVER_REC * sock, const char *uin)
{
	char *buffert = alloca(OVERHEAD+strlen(uin));
	char *buf, *flap;

	buf = flap = encode_snac(buffert, 0x30004);
	buf = writeb(buf, strlen(uin));
	buf = writes(buf, uin, strlen(uin));
	/* can add more here */
	snac_end(buf, flap);
	NOTICE("<- Sending add-buddy %s", uin);
	xwrite(sock, buffert, buf-buffert);
	buddy_setmode(uin, OFFLINE);
}

static void icq_reconnect(ICQ_SERVER_REC * sock, char *host, int port, char *cookie, int cookie_len)
{
	char *buffert = alloca(OVERHEAD + cookie_len);
	char *end;
	GIOChannel *c;

	if (sock->readtag >= 0)
		g_source_remove(sock->readtag);
	net_sendbuffer_destroy(sock->handle, TRUE);
	printtext(sock, NULL, MSGLEVEL_CLIENTNOTICE, "Reconnecting to %s:%d and sending %d-byte cookie", host, port, cookie_len);

	/* OK. This is a very very ugly hack.
	 * It blocks your irssi while connecting I think */
	/* If you discover strange bugs, I've probably forgot something here */
	c = net_connect(host, port, NULL);
	if (!c) {
		printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "reconnection failed.");
		return;
	}
	sock->handle = net_sendbuffer_create(c, 0);
	if (!sock->handle) {
		DEBUG("can't create sendbuffer");
		return;
	}
	sock->readtag = g_input_add(c,
			G_INPUT_READ, (GInputFunction) icq_parse_incoming, sock);
	if (sock->readtag <= 0) {
		DEBUG("can't add it to event loop");
		return;
	}

	end = encode_reconnect(buffert, cookie, cookie_len);
	xwrite(sock, buffert, end-buffert);

	DEBUG("OK. hope it works");
}

static void icq_send_data23(ICQ_SERVER_REC * sock)
{
	char buffert[OVERHEAD+40], *buf, *flap;

	buf = flap = encode_snac(buffert, 0x10000+23);
	buf = writes(buf, STR("\0\1\0\3\0\2\0\1\0\3\0\1\0\25\0\1\0\4\0\1\0\6\0\1\0\11\0\1\0\12\0\1"));
	snac_end(buf, flap);
	NOTICE("<- Sending data23");
	xwrite(sock, buffert, buf-buffert);
}

static void icq_send_clientready(ICQ_SERVER_REC * sock)
{
	char buffert[OVERHEAD+70], *buf, *flap;

	buf = flap = encode_snac(buffert, 0x10002);
	buf = writes(buf, STR("\x00\x01\x00\x03\x00\x04\x06\x86\x00\x02\x00\x01\x00\x04\x00\x01\x00\x03\x00\x01\x00\x04\x00\x01\x00\x04\x00\x01\x00\x04\x00\x01\x00\x09\x00\x01\x00\x04\x00\x01\x00\x0a\x00\x01\x00\x04\x00\x01\x00\x0b\x00\x01\x00\x04\x00\x01"));
	snac_end(buf, flap);
	NOTICE("<- Sending client-ready");
	xwrite(sock, buffert, buf-buffert);
}

static void icq_send_hashreply(ICQ_SERVER_REC * sock)
{
	char buffert[OVERHEAD+30], *buf, *flap;
	buf = flap = encode_snac(buffert, 0x10020);
	buf = writes(buf, STR("\x00\x10\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e"));
	snac_end(buf, flap);
	NOTICE("<- Sending hash-reply");
	xwrite(sock, buffert, buf-buffert);
}
			
void icq_send_setstatus(ICQ_SERVER_REC * sock, int status)
{
	char buffert[OVERHEAD+10], *buf, *flap, *tlv;
	buf = flap = encode_snac(buffert, 0x1001E);
	buf = tlv = tlv_begin(buf, 6);
	buf = writel(buf, status);
	tlv_end(buf, tlv);
	snac_end(buf, flap);
	DEBUG("<- Sending set-status %d", status);
	xwrite(sock, buffert, buf-buffert);
	my_status = status;
}

static int process_packet(ICQ_SERVER_REC *sock, char *buf, char *end, int channel)
{
	DEBUG("got %d byte packet on channel %d", end-buf, channel);

	switch (channel) {
	case 4:
	{
		char *cookie = NULL;
		int cookie_len = 0;
		char redir[100] = "";
		int port;
		int type, len;
		char *data;

		NOTICE("=> got redirection");

		while ((buf = read_tlv(buf, end, &type, &data, &len))) {
			DEBUG("tlv type %d len %d", type, len);
			switch (type) {
			/* case 1: uin */
			case 6:
				cookie = data;
				cookie_len = len;
				break;
			case 5:
				memcpy(redir,data,len);
				redir[len] = 0;
				DEBUG("redir: %s", redir);
				break;
			case 8:
				DEBUG("error code %d", readw(data));
				break;
			case 4:
				{
					char ch = data[len];
					data[len] = '\0';
					printtext(sock, NULL, MSGLEVEL_PUBLIC, "%s", data);
					data[len] = ch;
				}
				break;
			}
		}
		if (!cookie) {
			printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "Reconnect: Got no cookie!");
			return -1;
		}
		buf = strchr(redir,':');
		port = buf ? atoi(buf+1) : 0;
		if (!port) {
			printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "Reconnect: Got no redirection!");
			return -1;
		}
		*buf = '\0';

		icq_reconnect(sock, redir, port, cookie, cookie_len);
		break;
	}
	case 2:
	{
		int type;

		buf = decode_snac(buf, end, &type);
		if (!buf) {
			NOTICE("snac packet too short! (%d)", end - buf);
			return -1;
		}
		switch (type) {
		case 0x10003:
			NOTICE("=> Got server-ready");
			icq_send_data23(sock);
			icq_send_clientready(sock);
			break;

		case 0x1001F:
			NOTICE("=> Got hash-request");
			icq_send_hashreply(sock);
			break;

		case 0x40007:
			NOTICE("=> incoming message");
			{
				char uin[MAX_UIN];
				guint32 ip = -1;
				buf = decode_msg(buf, end, uin, &ip);
				if (buf) {
					/* change \r\n -> \n */
					char *p, *q;
					for (q = p = buf; *p; p++)
						if (*p != '\r') *q++ = *p;
					*q = '\0';
					query_updateip(sock, uin, ip);
					signal_emit("message private", 4, sock, buf, buddy_getalias(uin), "address?");
				}
			}
			break;
		case 0x1000f: /* my status */
			NOTICE("=> got my-status");
			{
				int mode = OFFLINE;
				buf = extract_user_info(buf, end, NULL, NULL, &mode);
				if (!buf) return -1;
				if (mode == 0) {
					sock->usermode_away = FALSE;
					g_free_and_null(sock->away_reason);
				} else {
					sock->usermode_away = TRUE;
					g_free(sock->away_reason);
					sock->away_reason = strdup(modestring(mode));
				}
				signal_emit("away mode changed", 1, sock);
			}
			break;
		case 0x3000c: /* offline */
		case 0x3000b: /* online */
			DEBUG("=> got mode-change");
			{
				int mode = OFFLINE;
				char uin[MAX_UIN];
				guint32 ip;

				buf = extract_user_info(buf, end, uin, &ip, &mode);
				if (!buf) return -1;
				query_updateip(sock, uin, ip);
				DEBUG("%s blev %d", uin, mode);
				if (mode == buddy_getmode(uin))
					break;
				buddy_setmode(uin, mode);
				signal_emit("nick mode change", 3, sock, buddy_getalias(uin), modestring(mode));
			}
			break;
			
		case 0xb0002:
			NOTICE("=> report-interval");
			printtext(sock, NULL, MSGLEVEL_CLIENTNOTICE, "login accepted");
			sock->connected = TRUE;
			signal_emit("event connected", 1, sock);
			buddy_forall((GFunc) icq_add_buddy, sock);
			icq_send_setstatus(sock, my_status);
			break;
		case 0x40001:
			NOTICE("=> msg-error");
			signal_emit("nick unknown", 2, sock, NULL);
			break;
		default:
			NOTICE("ignored snac type (%d,%d)", type>>16, type&0xFFFF);
		}
		break;
	}
	default:
		NOTICE("ignored %d byte packet on channel %d", end-buf, channel);
	}
	DEBUG("packed decoded successfully");
	return 0;
}

static void icq_send_login(ICQ_SERVER_REC *sock, const char *uin, const char *passwd)
{
	char *buf = alloca(OVERHEAD+60+strlen(uin)+strlen(passwd));
	char *end;

	NOTICE("Logging in as %s, passwd=%s", uin, passwd);
	end = encode_login(buf, uin, passwd);

	xwrite(sock, buf, end - buf);
}



void icq_check_buddies(ICQ_SERVER_REC *sock)
{
  //icq_check_buddies
  //ICQ_SERVER_REC * sock;
  buddy_forall((GFunc)icq_add_buddy, sock);
}

void icq_login(ICQ_SERVER_REC *sock) {
	const char *uin, *passwd;
	ICQ_SERVER_CONNECT_REC *conn;

	icq_parse_incoming(sock);

	conn = sock->connrec;
	uin = conn->nick;
	passwd = conn->password;
	if (!uin) {
		printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "No uin given!");
		return;
	}
	if (!passwd) {
		printtext(sock, NULL, MSGLEVEL_CLIENTERROR, "No password given!");
		return;
	}
	uin = buddy_getuin(uin);

	icq_send_login(sock, uin, passwd);
}

void icq_sendmsg(ICQ_SERVER_REC * server, const char *uin, const char *msg)
{
	char *buffert = alloca(OVERHEAD+strlen(uin)+strlen(msg));
	char *buf, *flap;
	int len;

	uin = buddy_getuin(uin);

	//DEBUG("Going to send '%s' to %s...", msg, uin);

	buf = flap = encode_snac(buffert, 0x40006);
	buf = encode_msg(buf, uin, msg);
	snac_end(buf, flap);
	len = buf-buffert;

	xwrite(server, buffert, len);
}

void icq_setstatus(ICQ_SERVER_REC * sock, const char *status)
{
	int code = -1;
	if (status && *status) {
		code = parse_away_mode(status);
		if (code == -1) {
			char *s = g_strjoinv(", ", (gchar **) away_modes);
			printtext(sock, NULL, MSGLEVEL_CLIENTNOTICE, "Away messages not implemented yet. Choose between %s for now", s);
			g_free(s);
			code = 1;
		}
	} else
		code = 0;
	/* TODO. Add 0x10000 to be invisible */
	icq_send_setstatus(sock, code);
}

void icq_addbuddy(ICQ_SERVER_REC * sock, const char *uin, const char *nick)
{
	add_buddy(uin, nick);
	icq_add_buddy(sock, uin);
}

static void sig_server_connected(ICQ_SERVER_REC * sock)
{
	if (!IS_ICQ_SERVER(sock))
		return;

	NOTICE("Reconnected");

	icq_login(sock);

	sock->readtag = g_input_add(net_sendbuffer_handle(sock->handle),
					G_INPUT_READ, (GInputFunction) icq_parse_incoming, sock);
}

static void event_login(ICQ_SERVER_REC * server, const char *data)
{
    /* Login OK */
	DEBUG("logged in");
	server->connected = TRUE;
	signal_emit("event connected", 1, server);
}

void icq_protocol_init(void)
{
	signal_add("server connected", (SIGNAL_FUNC) sig_server_connected);
	signal_add("icq event login", (SIGNAL_FUNC) event_login);
}

void icq_protocol_deinit(void)
{
	signal_remove("server connected", (SIGNAL_FUNC) sig_server_connected);
	signal_remove("icq event login", (SIGNAL_FUNC) event_login);
}
