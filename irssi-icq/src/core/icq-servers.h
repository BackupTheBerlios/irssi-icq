#ifndef __ICQ_SERVERS_H
#define __ICQ_SERVERS_H

#include "chat-protocols.h"
#include "servers.h"

#define SOCK_BUFSIZE 2024

/* returns ICQ_SERVER_REC if it's ICQ server, NULL if it isn't */
#define ICQ_SERVER(server) \
	PROTO_CHECK_CAST(SERVER(server), ICQ_SERVER_REC, chat_type, "ICQ")

#define ICQ_SERVER_CONNECT(conn) \
	PROTO_CHECK_CAST(SERVER_CONNECT(conn), ICQ_SERVER_CONNECT_REC, \
			 chat_type, "ICQ")

#define IS_ICQ_SERVER(server) \
	(ICQ_SERVER(server) ? TRUE : FALSE)

#define IS_ICQ_SERVER_CONNECT(conn) \
	(ICQ_SERVER_CONNECT(conn) ? TRUE : FALSE)

struct _ICQ_SERVER_CONNECT_REC {
#include "server-connect-rec.h"
};

#define STRUCT_SERVER_CONNECT_REC ICQ_SERVER_CONNECT_REC
struct _ICQ_SERVER_REC {
#include "server-rec.h"
	//unsigned char *cookie;
	//ICQ_SERVER_CONNECT_REC *redirected;
	char inbuf[SOCK_BUFSIZE];
	char *inbufp;
};

ICQ_SERVER_REC * icq_server_init_connect(ICQ_SERVER_CONNECT_REC * conn);
void icq_server_connect(ICQ_SERVER_REC * conn);
char *icq_server_get_channels(ICQ_SERVER_REC * server);

void icq_servers_init(void);
void icq_servers_deinit(void);

#endif
