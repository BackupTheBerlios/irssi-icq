/*
 icq-servers.c : irssi

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
#include "rawlog.h"
#include "net-sendbuffer.h"

#include "channels-setup.h"
#include "icq-servers.h"
#include "icq-protocol.h"
#include "commands.h"

ICQ_SERVER_REC * icq_server_init_connect(ICQ_SERVER_CONNECT_REC * conn)
{
	ICQ_SERVER_REC *server;

	g_return_val_if_fail(IS_ICQ_SERVER_CONNECT(conn), NULL);
	if (conn->address == NULL || *conn->address == '\0')
		return NULL;
	if (conn->nick == NULL || *conn->nick == '\0')
		return NULL;

	server = g_new0(ICQ_SERVER_REC, 1);
	server->chat_type = ICQ_PROTOCOL;

	server->connrec = conn;
	server_connect_ref(SERVER_CONNECT(conn));

	if (server->connrec->port <= 0)
		server->connrec->port = 5190;

	server_connect_init((SERVER_REC *) server);

	return server;
}

void icq_server_connect(ICQ_SERVER_REC * server)
{
	if (!server_start_connect((SERVER_REC *) server)) {
		server_connect_unref(SERVER_CONNECT(server->connrec));
		g_free(server);
		return;
	}

	server->inbufp = server->inbuf;
}

static void sig_server_disconnected(ICQ_SERVER_REC * server)
{
	if (!IS_ICQ_SERVER(server))
		return;

	if (server->handle != NULL) {
		/* disconnect it here, so the core won't try to leave it
		   later - ICQ has no quit messages so it doesn't really
		   matter if the server got our last messages or not */
		net_sendbuffer_destroy(server->handle, TRUE);
		server->handle = NULL;
	}
}

static int isnickflag_func(SERVER_REC *server, char flag)
{
	return FALSE;
}

static int ischannel_func(SERVER_REC * server, const char *data)
{
	return FALSE;
}

static const char *get_nick_flags(SERVER_REC *server)
{
	return "";
}

static void send_message(SERVER_REC * server, const char *target, const char *msg, int target_type)
{
	ICQ_SERVER_REC *icqserver;

	icqserver = ICQ_SERVER(server);
	g_return_if_fail(server != NULL);
	g_return_if_fail(target != NULL);

	icq_sendmsg(icqserver, target, msg);
}

static void channels_join(SERVER_REC *server, const char *channel, int automatic)
{
	signal_emit("error command", 2, GINT_TO_POINTER(CMDERR_ILLEGAL_PROTO));
}

static void sig_server_connected(ICQ_SERVER_REC * server)
{
	if (!IS_ICQ_SERVER(server))
		return;

	server->channels_join = channels_join;
	server->isnickflag = isnickflag_func;
	server->ischannel = ischannel_func;
	server->get_nick_flags = get_nick_flags;
	server->send_message = send_message;
}

void icq_servers_init(void)
{
	signal_add_first("server connected", (SIGNAL_FUNC) sig_server_connected);
	signal_add_last("server disconnected", (SIGNAL_FUNC) sig_server_disconnected);
}

void icq_servers_deinit(void)
{
	signal_remove("server connected", (SIGNAL_FUNC) sig_server_connected);
	signal_remove("server disconnected", (SIGNAL_FUNC) sig_server_disconnected);
}
