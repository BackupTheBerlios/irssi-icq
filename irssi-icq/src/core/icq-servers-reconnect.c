/*
 icq-servers-reconnect.c : irssi

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

#include "icq-servers.h"

static void sig_server_connect_copy(SERVER_CONNECT_REC ** dest, ICQ_SERVER_CONNECT_REC * src)
{
	ICQ_SERVER_CONNECT_REC *rec;

	g_return_if_fail(dest != NULL);
	if (!IS_ICQ_SERVER_CONNECT(src))
		return;

	rec = g_new0(ICQ_SERVER_CONNECT_REC, 1);
	rec->chat_type = ICQ_PROTOCOL;
	*dest = (SERVER_CONNECT_REC *) rec;
}

void icq_servers_reconnect_init(void)
{
	signal_add("server connect copy", (SIGNAL_FUNC) sig_server_connect_copy);
}

void icq_servers_reconnect_deinit(void)
{
	signal_remove("server connect copy", (SIGNAL_FUNC) sig_server_connect_copy);
}
