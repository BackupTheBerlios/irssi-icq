/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZPending function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZPending.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZPending.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZPending_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZPending.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

int ZPending()
{
	int retval;
	
	if (ZGetFD() < 0) {
		errno = ZERR_NOPORT;
		return (-1);
	}
	
	if ((retval = Z_ReadEnqueue()) != ZERR_NONE) {
		errno = retval;
		return (-1);
	} 
	
	return(ZQLength());
}
