#include "module.h"
#include "signals.h"
#include "icq-servers.h"
#include "fe-common/core/printtext.h"
#include "levels.h"
#include "icq-buddy.h"
#include "settings.h"
#include "misc.h"

struct buddy {
	char *uin;
	char *alias;
	int mode;
	guint32 ip;
};

GSList *buddies = NULL;

void add_buddy(const char *uin, const char *alias)
{
	struct buddy *new = g_new(struct buddy, 1);
	new->uin = strdup(uin);
	new->alias = strdup(alias);
	buddies = g_slist_prepend(buddies, new);
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
	return strcmp(alias, try->alias);
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

const char *modestring(int status)
{
	static char unknown[30];

	switch (status) {
		case OFFLINE: return "offline";
		case 0: return "online";
		case 1: return "away";
		case 4: return "N/A-licq";
		case 5: return "N/A";
		case 17: return "occupied";
		case 19: return "do not disturb";
		case 32: return "free for chat";
		default:
			snprintf(unknown, sizeof unknown, "(%d)", status);
			return unknown;
	}
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
