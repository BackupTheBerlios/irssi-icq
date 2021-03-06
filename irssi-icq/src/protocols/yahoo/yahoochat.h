/**
 * @file yahoochat.h The Yahoo! protocol plugin, chat and conference stuff
 *
 * gaim
 *
 * Copyright (C) 2003
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _YAHOOCHAT_H_
#define _YAHOOCHAT_H_

void yahoo_process_conference_invite(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_decline(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_logon(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_logoff(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_conference_message(GaimConnection *gc, struct yahoo_packet *pkt);

void yahoo_process_chat_online(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_logout(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_join(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_exit(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_message(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_addinvite(GaimConnection *gc, struct yahoo_packet *pkt);
void yahoo_process_chat_goto(GaimConnection *gc, struct yahoo_packet *pkt);

void yahoo_c_leave(GaimConnection *gc, int id);
int yahoo_c_send(GaimConnection *gc, int id, const char *what);
GList *yahoo_c_info(GaimConnection *gc);
void yahoo_c_join(GaimConnection *gc, GHashTable *data);
void yahoo_c_invite(GaimConnection *gc, int id, const char *msg, const char *name);

void yahoo_chat_goto(GaimConnection *gc, const char *name);

#endif /* _YAHOO_CHAT_H_ */
