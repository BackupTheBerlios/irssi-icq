/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZGetWGPort function.
 *
 *	Created by:	Robert French
 *
 *	$Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetWGPort.c,v $
 *	$Author: dothebart $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */
/* $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetWGPort.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $ */

#ifndef lint
static char rcsid_ZGetWGPort_c[] = "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/ZGetWGPort.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

#include <internal.h>

int ZGetWGPort()
{
    char *envptr, name[128];
    FILE *fp;
    int wgport;
	
    envptr = getenv("WGFILE");
    if (!envptr) {
	(void) sprintf(name, "/tmp/wg.%d", getuid());
	envptr = name;
    } 
    if (!(fp = fopen(envptr, "r")))
	return (-1);

    /* if fscanf fails, return -1 via wgport */
    if (fscanf(fp, "%d", &wgport) != 1)
	    wgport = -1;

    (void) fclose(fp);

    return (wgport);
}
