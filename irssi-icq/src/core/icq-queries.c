/*
 icq-queries.c : irssi

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
#include "misc.h"

#include "icq-servers.h"
#include "icq-queries.h"
#include "icq-buddy.h"

static void query_update_topic(QUERY_REC *query, const char *uin, guint32 ip)
{
	if (ip && ip != -1) {
		char *address = g_strdup_printf("%s@%d.%d.%d.%d", uin, ip>>24, (ip>>16)&255, (ip>>8)&255, ip&255);
		if (!query->address || strcmp(query->address, address))
			query_change_address(query, address);
		g_free(address);
	} else if (!query->address) {
		query_change_address(query, uin);
	}
}

QUERY_REC *icq_query_create(const char *server_tag, const char *nick, int automatic)
{
	QUERY_REC *rec;

	g_return_val_if_fail(nick != NULL, NULL);

	rec = g_new0(QUERY_REC, 1);
	rec->chat_type = ICQ_PROTOCOL;
	rec->name = g_strdup(nick);
	rec->server_tag = g_strdup(server_tag);
	query_init(rec, automatic);

	nick = buddy_getuin(nick);
	query_update_topic(rec, nick, buddy_getip(nick));
	return rec;
}

void query_updateip(ICQ_SERVER_REC *server, const char *nick, guint32 ip)
{
	const char *uin = buddy_getuin(nick);
	nick = buddy_getalias(nick);

	buddy_setip(uin, ip);
	if (server) {
		/* save nick's address to query */
		QUERY_REC *query = icq_query_find(server, nick);

		if (query) query_update_topic(query, uin, ip);
	}
}

void icq_queries_init(void)
{
	//signal_add_last("message private", (SIGNAL_FUNC) event_privmsg);
}

void icq_queries_deinit(void)
{
	//signal_remove("message private", (SIGNAL_FUNC) event_privmsg);
}
