/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZCompareUIDPred function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZCmpUIDP.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZCmpUIDP.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZCompareUIDPred_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZCmpUIDP.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

int ZCompareUIDPred(notice, uid)
    ZNotice_t *notice;
    void *uid;
{
    return (ZCompareUID(&notice->z_uid, (ZUnique_Id_t *) uid));
}

int ZCompareMultiUIDPred(notice, uid)
    ZNotice_t *notice;
    void *uid;
{
    return (ZCompareUID(&notice->z_multiuid, (ZUnique_Id_t *) uid));
}
