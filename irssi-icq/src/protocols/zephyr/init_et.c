/*
 * $Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/init_et.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $
 * $Source: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/init_et.c,v $
 * $Locker:  $
 *
 * Copyright 1986, 1987, 1988 by MIT Information Systems and
 *	the MIT Student Information Processing Board.
 *
 * For copyright info, see mit-sipb-copyright.h.
 */

#include <sysdep.h>
#include "error_table.h"
#include "mit-sipb-copyright.h"

#ifndef	lint
static const char rcsid_init_et_c[] =
    "$Header: /home/xubuntu/berlios_backup/github/tmp-cvs/irssi-icq/Repository/irssi-icq/src/protocols/zephyr/init_et.c,v 1.1 2003/11/19 12:41:52 dothebart Exp $";
#endif

struct foobar {
    struct et_list etl;
    struct error_table et;
};

extern struct et_list * _et_list;

int init_error_table(msgs, base, count)
    const char * const * msgs;
    int base;
    int count;
{
    struct foobar * new_et;

    if (!base || !count || !msgs)
	return 0;

    new_et = (struct foobar *) malloc(sizeof(struct foobar));
    if (!new_et)
	return errno;	/* oops */
    new_et->etl.table = &new_et->et;
    new_et->et.msgs = msgs;
    new_et->et.base = base;
    new_et->et.n_msgs= count;

    new_et->etl.next = _et_list;
    _et_list = &new_et->etl;
    return 0;
}
