#ifndef __ICQ_COMMANDS_H
#define __ICQ_COMMANDS_H

#include "commands.h"

#define command_bind_icq(cmd, section, signal) \
        command_bind_proto(cmd, ICQ_PROTOCOL, section, signal)
#define command_bind_icq_first(cmd, section, signal) \
        command_bind_proto_first(cmd, ICQ_PROTOCOL, section, signal)
#define command_bind_icq_last(cmd, section, signal) \
        command_bind_proto_last(cmd, ICQ_PROTOCOL, section, signal)

/* Simply returns if server isn't for ICQ protocol. Prints ERR_NOT_CONNECTED
   error if there's no server or server isn't connected yet */
#define CMD_ICQ_SERVER(server) \
	G_STMT_START { \
          if (server != NULL && !IS_ICQ_SERVER(server)) \
            return; \
          if (server == NULL || !(server)->connected) \
            cmd_return_error(CMDERR_NOT_CONNECTED); \
	} G_STMT_END

void icq_commands_init(void);
void icq_commands_deinit(void);

#endif
