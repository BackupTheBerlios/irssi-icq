/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZFormatSmallRawNotice function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZFmtSmRaw.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZFmtSmRaw.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZFormatRawNotice_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZFmtSmRaw.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

Code_t ZFormatSmallRawNotice(notice, buffer, ret_len)
    ZNotice_t *notice;
    ZPacket_t buffer;
    int *ret_len;
{
    Code_t retval;
    int hdrlen;
    
    if ((retval = Z_FormatRawHeader(notice, buffer, Z_MAXHEADERLEN,
				    &hdrlen, NULL, NULL)) != ZERR_NONE)
	return (retval);

    *ret_len = hdrlen+notice->z_message_len;

    if (*ret_len > Z_MAXPKTLEN)
	return (ZERR_PKTLEN);

    (void) memcpy(buffer+hdrlen, notice->z_message, notice->z_message_len);

    return (ZERR_NONE);
}
