/*
 icq-core.c : ICQ protocol module for irssi

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

#include "chat-protocols.h"
#include "chatnets.h"
#include "servers-setup.h"
#include "channels-setup.h"

#include "icq-commands.h"
#include "icq-servers.h"
#include "icq-queries.h"
#include "icq-protocol.h"
#include "icq-buddy.h"

#include "settings.h"

void icq_session_init(void);
void icq_session_deinit(void);

void icq_servers_reconnect_init(void);
void icq_servers_reconnect_deinit(void);

static CHATNET_REC *create_chatnet(void)
{
	return g_new0(CHATNET_REC, 1);
}

static SERVER_SETUP_REC *create_server_setup(void)
{
	return g_new0(SERVER_SETUP_REC, 1);
}

static SERVER_CONNECT_REC *create_server_connect(void)
{
	return g_new0(SERVER_CONNECT_REC, 1);
}

static void destroy_server_connect(SERVER_CONNECT_REC * conn)
{
}

void icq_core_init(void)
{
	CHAT_PROTOCOL_REC *rec;

	settings_add_str("misc", "buddy_file", "~/.irssi/icq_config");

	rec = g_new0(CHAT_PROTOCOL_REC, 1);
	rec->name = "ICQ";
	rec->fullname = "I Seek You";
	rec->chatnet = "icqnet";

	rec->case_insensitive = TRUE;

	rec->create_chatnet = create_chatnet;
	rec->create_server_setup = create_server_setup;
	/* this NULL here causes segfault with /CHANNEL commands */
	rec->create_channel_setup = NULL;
	rec->create_server_connect = create_server_connect;
	rec->destroy_server_connect = destroy_server_connect;

	rec->server_init_connect = (SERVER_REC * (*)(SERVER_CONNECT_REC *)) icq_server_init_connect;

	rec->server_connect = (void (*)(SERVER_REC*)) icq_server_connect;
	rec->channel_create = NULL;
	rec->query_create = (QUERY_REC * (*)(const char *, const char *, int))
		icq_query_create;

	chat_protocol_register(rec);
	g_free(rec);

	icq_servers_init();
	icq_servers_reconnect_init();
	icq_protocol_init();
	icq_commands_init();
	icq_queries_init();
	icq_session_init();
	read_buddy_file();

	module_register("icq", "core");
}

void icq_core_deinit(void)
{
	icq_servers_deinit();
	icq_servers_reconnect_deinit();
	icq_protocol_deinit();
	icq_commands_deinit();
	icq_queries_deinit();
	icq_session_deinit();

	destroy_buddy_list();

	signal_emit("chat protocol deinit", 1, chat_protocol_find("ICQ"));
	chat_protocol_unregister("ICQ");
}
