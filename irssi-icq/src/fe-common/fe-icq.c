/*
 fe-icq.c : irssi

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
#include "module-formats.h"
#include "signals.h"
#include "commands.h"
#include "servers-setup.h"
#include "levels.h"

#include "icq.h"
#include "icq-servers.h"
#include "icq-protocol.h"
#include "icq-buddy.h"
#include "icq-commands.h"

#include "printtext.h"
#include "themes.h"

static void sig_server_add_fill(SERVER_SETUP_REC *rec, GHashTable *optlist)
{
	char *value;

	value = g_hash_table_lookup(optlist, "icqnet");
	if (value != NULL) {
		g_free_and_null(rec->chatnet);
		if (*value != '\0') rec->chatnet = g_strdup(value);
	}
}

static void nick_modechange(ICQ_SERVER_REC *server, char *uin, char *mode)
{
	printformat(server, uin, MSGLEVEL_MODES, ICQTXT_MODECHANGE, uin, mode);
}

static void nick_unknown(ICQ_SERVER_REC *server, char *uin)
{
	printformat(server, uin, MSGLEVEL_MSGS, ICQTXT_NOSUCH);
}

static int code_restrict;

static void printinfo(ICQ_SERVER_REC *server, const char *nick)
{
	const char *uin;
	guint32 ip;
	char *numip;
	const char *mode;
	int code;

	uin = buddy_getuin(nick);
	code = buddy_getmode(uin);
	if (code_restrict >= 0 && code_restrict != code)
		return;
	nick = buddy_getalias(uin);
	ip = buddy_getip(uin);
	numip = g_strdup_printf("%d.%d.%d.%d", ip>>24, (ip>>16)&255, (ip>>8)&255, ip&255);
	mode = modestring(code);

	printformat(server, uin, MSGLEVEL_CRAP, ICQTXT_WHO, nick, uin, numip, mode);

	g_free(numip);
}

static void cmd_whois(const char *data, ICQ_SERVER_REC * server)
{
	CMD_ICQ_SERVER(server);
	
	if (!data || !*data)
		code_restrict = -2;
	else {
		code_restrict = parse_away_mode(data);
		if (code_restrict < 0) {
			printinfo(server, data);
			return;
		}
	}

	buddy_forall((GFunc)printinfo, server);
}

void fe_icq_init(void)
{
	theme_register(fecommon_icq_formats);

	signal_add("nick mode change", (SIGNAL_FUNC) nick_modechange);
	signal_add("nick unknown", (SIGNAL_FUNC) nick_unknown);
	signal_add("server add fill", (SIGNAL_FUNC) sig_server_add_fill);

	command_bind_icq("whois", NULL, (SIGNAL_FUNC) cmd_whois);

	command_set_options("server add", "-icqnet");
	module_register("icq", "fe");
}

void fe_icq_deinit(void)
{
	command_unbind("whois", (SIGNAL_FUNC) cmd_whois);

	signal_remove("nick mode change", (SIGNAL_FUNC) nick_modechange);
	signal_remove("nick unknown", (SIGNAL_FUNC) nick_unknown);
	signal_remove("server add fill", (SIGNAL_FUNC) sig_server_add_fill);
}
