#ifndef __ICQ_QUERIES_H
#define __ICQ_QUERIES_H

#include "queries.h"
#include "icq-servers.h"

/* Returns ICQ_QUERY_REC if it's ICQ query, NULL if it isn't. */
#define ICQ_QUERY(query) \
	PROTO_CHECK_CAST(QUERY(query), QUERY_REC, chat_type, "ICQ")

#define IS_ICQ_QUERY(query) \
	(ICQ_QUERY(query) ? TRUE : FALSE)

QUERY_REC *icq_query_create(const char *server_tag, const char *nick, int automatic);

#define icq_query_find(server, name) \
	query_find(SERVER(server), name)

void icq_queries_init(void);
void icq_queries_deinit(void);
void query_updateip(ICQ_SERVER_REC *server, const char *nick, guint32 ip);

#endif
