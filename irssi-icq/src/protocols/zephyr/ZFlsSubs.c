/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZFlushSubscriptions function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZFlsSubs.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZFlsSubs.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#include <internal.h>

#ifndef lint
static const char rcsid_ZFlushSubscriptions_c[] = "$Id: ZFlsSubs.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

Code_t ZFlushSubscriptions()
{
	register int i;
	
	if (!__subscriptions_list)
		return (ZERR_NONE);

	for (i=0;i<__subscriptions_num;i++) {
		free(__subscriptions_list[i].zsub_class);
		free(__subscriptions_list[i].zsub_classinst);
		free(__subscriptions_list[i].zsub_recipient);
	}
	
	free((char *)__subscriptions_list);

	__subscriptions_list = 0;
	__subscriptions_num = 0;

	return (ZERR_NONE);
}

