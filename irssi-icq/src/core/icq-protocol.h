#ifndef __ICQ_PROTOCOL_H
#define __ICQ_PROTOCOL_H

#define ICQ_PROTOCOL_LEVEL 1

void icq_sendmsg(ICQ_SERVER_REC * server, const char *uin, const char *msg);
void icq_send_setstatus(ICQ_SERVER_REC * sock, int status);
void icq_addbuddy(ICQ_SERVER_REC * sock, const char *uin, const char *nick);

void icq_protocol_init(void);
void icq_protocol_deinit(void);

#endif
