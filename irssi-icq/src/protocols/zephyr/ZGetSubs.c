/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZGetSubscriptions function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetSubs.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetSubs.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZGetSubscriptions_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetSubs.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

#define min(a,b) ((a)<(b)?(a):(b))
	
Code_t ZGetSubscriptions(subscription, numsubs)
    ZSubscription_t *subscription;
    int *numsubs;
{
    int i;
	
    if (!__subscriptions_list)
	return (ZERR_NOSUBSCRIPTIONS);

    if (__subscriptions_next == __subscriptions_num)
	return (ZERR_NOMORESUBSCRIPTIONS);
	
    for (i=0;i<min(*numsubs, __subscriptions_num-__subscriptions_next);i++)
	subscription[i] = __subscriptions_list[i+__subscriptions_next];

    if (__subscriptions_num-__subscriptions_next < *numsubs)
	*numsubs = __subscriptions_num-__subscriptions_next;

    __subscriptions_next += *numsubs;
	
    return (ZERR_NONE);
}
