/*
 module-formats.c : irssi

    Copyright (C) 2001 Timo Sirainen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "module.h"
#include "formats.h"

FORMAT_REC fecommon_icq_formats[] = {
	{ MODULE_NAME, "ICQ", 0 },

	/* ---- */
	{ NULL, "Misc", 0 },

	{ "mode_change", "{nick $0} changed mode to {mode $1}", 2, { 0, 0 } },
	{ "nosuchnick", "No such nick", 0, { } },
	/* nick, uin, numip, mode */
	{ "who", "{nick $[!11]0} {mode $[!8]3} {nickhost $[-10]1@$2}", 4, { 0, 0, 0, 0 } },

	{ NULL, NULL, 0 }
};
