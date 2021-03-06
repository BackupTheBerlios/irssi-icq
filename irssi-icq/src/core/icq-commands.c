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

#include "icq-commands.h"
#include "icq-servers.h"
#include "icq-protocol.h"
#include "icq-buddy.h"
#include "fe-common/core/printtext.h"
#include "levels.h"
#include "signals.h"

static void cmd_addbuddy(const char *data, ICQ_SERVER_REC * server)
{
	char *uin, *nick = NULL;
	void *free_arg;

	CMD_ICQ_SERVER(server);

	if (!cmd_get_params(data, &free_arg, 2, &uin, &nick))
		return;

	if (nick && *nick)
		icq_addbuddy(server, uin, nick);
	else
		cmd_param_error(CMDERR_NOT_ENOUGH_PARAMS);
	cmd_params_free(free_arg);
}

static void cmd_away(const char *status, ICQ_SERVER_REC * sock)
{
	int code = -1;

	CMD_ICQ_SERVER(sock);
	printtext(sock, NULL,  MSGLEVEL_CLIENTNOTICE,"blabla");
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

static void cmd_version(const char *data, ICQ_SERVER_REC *sock)
{
  //  GaimPluginInfo* info=ICQ_SERVER_REC;


}

void icq_commands_init(void)
{
	command_bind_icq("addbuddy", NULL, (SIGNAL_FUNC) cmd_addbuddy);
	//command_bind_icq("away", NULL, (SIGNAL_FUNC) cmd_away);
	signal_add("away mode changed", (SIGNAL_FUNC) cmd_away);
	// "awaylog show", LOG_REC, int away_msgs, int filepos -> do we need that too?
	//    command_bind_icq("save", NULL, (SIGNAL_FUNC) icq_save_buddy_file);
	signal_add("setup saved", (SIGNAL_FUNC)icq_save_buddy_file);
	signal_add("setup reread",(SIGNAL_FUNC)icq_reread_buddy_file);
	/*    command_bind_icq("connect", NULL, (SIGNAL_FUNC) icq_check_buddies);*/
	command_set_options("connect", "+icqnet");
}

void icq_commands_deinit(void)
{
  signal_remove("setup saved", (SIGNAL_FUNC)icq_save_buddy_file);
  signal_remove("setup reread",(SIGNAL_FUNC)icq_reread_buddy_file);
  //    command_unbind("save", (SIGNAL_FUNC) icq_save_buddy_file);
  /*    command_unbind("connect", (SIGNAL_FUNC) icq_check_buddies);*/
  command_unbind("addbuddy", (SIGNAL_FUNC) cmd_addbuddy);
  //command_unbind("away", (SIGNAL_FUNC) cmd_away);
  signal_remove("away mode changed", (SIGNAL_FUNC) cmd_away);
}
