/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZSetDestAddr function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZSetDest.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZSetDest.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZSetDestAddr_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZSetDest.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

Code_t ZSetDestAddr(addr)
	struct	sockaddr_in *addr;
{
	__HM_addr = *addr;

	__HM_set = 1;
	
	return (ZERR_NONE);
}
