#include "module.h"
#include "signals.h"
#include "icq-servers.h"
#include "fe-common/core/printtext.h"
#include "levels.h"
#include "icq-buddy.h"
#include "settings.h"
#include "misc.h"
#include "core/queries.h"
struct buddy {
	char *uin;
	char *alias;
	int mode;
	guint32 ip;
};

GSList *buddies = NULL;

void add_buddy(const char *uin, const char *alias)
{
  QUERY_REC *query;
  struct buddy *new = g_new(struct buddy, 1);
  new->uin = strdup(uin);
  new->alias = strdup(alias);
  buddies = g_slist_prepend(buddies, new);
  
  query=query_find(NULL,uin);
  if (query)
	{
	  g_free(query->visible_name);

	  query->visible_name=g_strdup(alias);
	  query->name=g_strdup(alias);
	  signal_emit("query nick changed", 2, query, uin);
	  signal_emit("window item name changed", 1, query);
	}
}

void destroy_buddy_list(void)
{
	g_slist_foreach(buddies, (GFunc)g_free, NULL);
	g_slist_free(buddies);
	buddies = NULL;
}

static int compare_uin(struct buddy *try, const char *uin)
{
	return strcmp(uin, try->uin);
}
static int compare_alias(struct buddy *try, const char *alias)
{
	return strcasecmp(alias, try->alias);
}

static struct buddy *get_buddy(const char *uin)
{
	GSList *item = g_slist_find_custom(buddies, (char *)uin, (GCompareFunc)compare_uin);
	if (!item) return NULL;
	return item->data;
}

const char *buddy_getalias(const char *uin)
{
	struct buddy *item = get_buddy(uin);
	if (!item) return uin;
	return item->alias;
}

const char *buddy_getuin(const char *alias)
{
	GSList *item = g_slist_find_custom(buddies, (char *)alias, (GCompareFunc)compare_alias);
	if (!item) return alias;
	return ((struct buddy*)item->data)->uin;
}

void buddy_forall(GFunc f, gpointer user_data)
{
	GSList *list = buddies;
	while (list) {
		struct buddy* item = list->data;
		f(user_data, item->uin);
		list = g_slist_next(list);
	}
}

int buddy_getmode(const char *uin)
{
	struct buddy *item = get_buddy(uin);
	if (!item) return -1;
	return item->mode;
}
int buddy_setmode(const char *uin, int mode)
{
	struct buddy *item = get_buddy(uin);
	if (!item) return -1;
	return (item->mode = mode);
}

guint32 buddy_getip(const char *uin)
{
	struct buddy *item = get_buddy(uin);
	if (!item) return -1;
	return item->ip;
}

/* Somebody mentioned ipv6? :) */

int buddy_setip(const char *uin, guint32 ip)
{
	struct buddy *item = get_buddy(uin);
	if (!item) return -1;
	item->ip = ip;
	return 0;
}

static void trim(char *src)
{
	char *dst = src, *last = src + 666666;

	while (*src) {
		if (!isspace(*src)) {
			if (src > last)
				*dst++ = ' ';
			*dst++ = *src++;
			last = src;
		} else
			src++;
	}
	*dst = '\0';
}
/*
  void icq_flushbuddy(const char *uin)
  {
  
  }
*/

void read_buddy_file(void)
{
	FILE *stream;
	char line[80];
	const char *file = settings_get_str("buddy_file");
	//const char *file = "~/.irssi/icq_config";
	char *file_expanded;

	if (!file || !*file) return;
	file_expanded = convert_home(file);
	stream = fopen(file_expanded, "r");
	if (!stream) return;
	while (fgets(line, sizeof line, stream)) {
		char *buddy;
		trim(line);
		buddy = strchr(line, ' ');
		if (!buddy) continue;
		*buddy = '\0';
		buddy++;
		if (strspn(line, "0123456789") != strlen(line)) continue;
		add_buddy(line, buddy);
	}
	fclose(stream);
	printtext(NULL, NULL, MSGLEVEL_CLIENTNOTICE, "%d aliases read from %s", g_slist_length(buddies), file_expanded);
	g_free(file_expanded);
}

void icq_reread_buddy_file(void)
{
   destroy_buddy_list();
   read_buddy_file();
}


void icq_savebuddy(FILE *stream, const char *uin)
{
  fprintf(stream, "%s %s\n", uin, buddy_getalias(uin));
  printtext(NULL, NULL, MSGLEVEL_CLIENTNOTICE, " %s %s ",uin, buddy_getalias(uin));
}

void icq_save_buddy_file(void)
{
	FILE *stream;
	const char *file = settings_get_str("buddy_file");
	//const char *file = "~/.irssi/icq_config";
	char *file_expanded;

	if (!file || !*file) return;
	file_expanded = convert_home(file);
	stream = fopen(file_expanded, "w");
	if (!stream) return;
    buddy_forall((GFunc)icq_savebuddy,stream);
    if (stream)	
    {
	  fclose(stream);
	  printtext(NULL, NULL, MSGLEVEL_CLIENTNOTICE, "%d aliases saved to %s", g_slist_length(buddies), file_expanded);
	}
	g_free(file_expanded);
}

char *read_conf_option(const char *opt)
{
	FILE *stream;
	char line[80];
	const char *file = settings_get_str("buddy_file");
	//const char *file = "~/.irssi/icq_config";
	char *file_expanded;

	if (!file || !*file) return NULL;
	file_expanded = convert_home(file);
	stream = fopen(file_expanded, "r");
	g_free(file_expanded);
	if (!stream) return NULL;
	while (fgets(line, sizeof line, stream)) {
		char *buddy;
		trim(line);
		buddy = strchr(line, ' ');
		if (!buddy) continue;
		*buddy = '\0';
		buddy++;
		if (!strcmp(line, opt)) {
			fclose(stream);
			return g_strdup(buddy);
		}
	}
	fclose(stream);
	return NULL;
}

// Status constants
// Statuses must be checked in the following order:
//  DND, Occupied, NA, Away, Online
const unsigned short ICQ_STATUS_OFFLINE            = 0xFFFF;
const unsigned short ICQ_STATUS_ONLINE             = 0x0000;
const unsigned short ICQ_STATUS_AWAY               = 0x0001;
const unsigned short ICQ_STATUS_DND                = 0x0002;
const unsigned short ICQ_STATUS_NA                 = 0x0004;
const unsigned short ICQ_STATUS_OCCUPIED           = 0x0010;
const unsigned short ICQ_STATUS_FREEFORCHAT        = 0x0020;
const char *modestring(int status)
{
	static char unknown[30];
	static char allmode[60];
	char *mode;
	int lowerstat=status&0XFF;

	switch (lowerstat) 
	  {
	  case 219:
	  case OFFLINE: mode="offline"; break;
	  case 0: mode="online"; break;
	  case 1: mode="away"; break;
	  case 2: mode="do not disturb"; break;
	  case 4: mode="N/A-licq"; break;
	  case 5: mode="N/A"; break;
	  case 17: mode="occupied"; break;
	  case 19: mode="do not disturb"; break;
	  case 32: mode="free for chat"; break;
	  default:
		snprintf(unknown, sizeof unknown, "damn.(%d)", lowerstat);
		mode=unknown;
		break;
	}
	if (status & 0x100) /* stealth-flag */
	  {
		snprintf(allmode, sizeof allmode, "%s Invisible", mode);
		return allmode;
	  }
	return mode;
}

const char *away_modes[] =
	{ "online", "away", "na", "occ", "dnd", "ffc", "offline", NULL };

int parse_away_mode(const char *status)
{
	static const int codes[] = { 0, 1, 5, 17, 19, 32, OFFLINE };
	int i, code = -1;
	char *endp;
	for (i = 0; away_modes[i]; i++)
		if (!strcmp(status, away_modes[i])) return codes[i];
	code = strtol(status, &endp, 0);
	if (*endp) return -1;
	return code;
}

