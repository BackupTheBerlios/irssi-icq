#include <stdio.h>
#include "internal.h"
#include "account.h"
#include "conversation.h"
#include "util.h"
#include "prefs.h"
#include "ft.h"
#include "server.h"
#include "log.h"
#include "debug.h"
#include "blist.h"
#include "request.h"
//->connection.c
static GList *connections = NULL;
static GList *connections_connecting = NULL;
//static GaimConnectionUiOps *connection_ui_ops = NULL;

///---------------------------------------
// connection.c
void
gaim_connection_set_display_name(GaimConnection *gc, const char *name)
{
	g_return_if_fail(gc != NULL);

	if (gc->display_name != NULL)
		g_free(gc->display_name);

	gc->display_name = (name == NULL ? NULL : g_strdup(name));
}
GaimAccount *
gaim_connection_get_account(const GaimConnection *gc)
{
	g_return_val_if_fail(gc != NULL, NULL);

	return gc->account;
}




void
gaim_connection_update_progress(GaimConnection *gc, const char *text,
								size_t step, size_t count)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc   != NULL);
	g_return_if_fail(text != NULL);
	g_return_if_fail(step < count);
	g_return_if_fail(count > 1);

	ops = gaim_get_connection_ui_ops();

	if (ops != NULL && ops->connect_progress != NULL)
		ops->connect_progress(gc, text, step, count);
}

//-------------------------------------------------------
//->connections.c
GList *
gaim_connections_get_all(void)
{
	return connections;
}

gboolean
gaim_connection_disconnect_cb(gpointer data)
{
	GaimAccount *account = data;
	GaimConnection *gc = gaim_account_get_connection(account);

	if(gc)
		gaim_connection_disconnect(gc);

	return FALSE;
}
void
gaim_connection_error(GaimConnection *gc, const char *text)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc   != NULL);
	g_return_if_fail(text != NULL);

	/* If we've already got one error, we don't need any more */
	if (gc->disconnect_timeout)
		return;

	ops = gaim_get_connection_ui_ops();

	if (ops != NULL) {
		if (ops->report_disconnect != NULL)
			ops->report_disconnect(gc, text);

		if (ops->disconnected != NULL)
			ops->disconnected(gc);
	}

	gc->disconnect_timeout = g_timeout_add(0, gaim_connection_disconnect_cb,
			gaim_connection_get_account(gc));
}
/*
 * d:)->-< 
 *
 * d:O-\-<
 * 
 * d:D-/-<
 *
 * d8D->-< DANCE!
 */

void
gaim_connection_set_state(GaimConnection *gc, GaimConnectionState state)
{
	GaimConnectionUiOps *ops;

	g_return_if_fail(gc != NULL);

	if (gc->state == state)
		return;

	gc->state = state;

	ops = gaim_get_connection_ui_ops();

	if (gc->state == GAIM_CONNECTING) {
		connections_connecting = g_list_append(connections_connecting, gc);
	}
	else {
		connections_connecting = g_list_remove(connections_connecting, gc);
	}

	if (gc->state == GAIM_CONNECTED) {
		GaimBlistNode *gnode,*cnode,*bnode;
		GList *wins;
		GList *add_buds=NULL;

		/* Set the time the account came online */
		time(&gc->login_time);

		if (ops != NULL && ops->connected != NULL)
			ops->connected(gc);

		gaim_blist_show();
		gaim_blist_add_account(gc->account);

		/*
		 * XXX This is a hack! Remove this and replace it with a better event
		 *     notification system.
		 */
		for (wins = gaim_get_windows(); wins != NULL; wins = wins->next) {
			GaimWindow *win = (GaimWindow *)wins->data;
			gaim_conversation_update(gaim_window_get_conversation_at(win, 0),
									 GAIM_CONV_ACCOUNT_ONLINE);
		}

		gaim_signal_emit(gaim_connections_get_handle(), "signed-on", gc);

		/*		system_log(log_signon, gc, NULL,
				   OPT_LOG_BUDDY_SIGNON | OPT_LOG_MY_SIGNON);
		*/////
#if 0
		/* away option given? */
		if (opt_away) {
			away_on_login(opt_away_arg);
			/* don't do it again */
			opt_away = 0;
		} else if (awaymessage) {
			serv_set_away(gc, GAIM_AWAY_CUSTOM, awaymessage->message);
		}
		if (opt_away_arg != NULL) {
			g_free(opt_away_arg);
			opt_away_arg = NULL;
		}
#endif

		/* let the prpl know what buddies we pulled out of the local list */
		for (gnode = gaim_get_blist()->root; gnode; gnode = gnode->next) {
			if(!GAIM_BLIST_NODE_IS_GROUP(gnode))
				continue;
			for(cnode = gnode->child; cnode; cnode = cnode->next) {
				if(!GAIM_BLIST_NODE_IS_CONTACT(cnode))
					continue;
				for(bnode = cnode->child; bnode; bnode = bnode->next) {
					GaimBuddy *b;
					if(!GAIM_BLIST_NODE_IS_BUDDY(bnode))
						continue;

					b = (GaimBuddy *)bnode;
					if(b->account == gc->account) {
						add_buds = g_list_append(add_buds, b->name);
					}
				}
			}
		}

		if(add_buds) {
			serv_add_buddies(gc, add_buds);
			g_list_free(add_buds);
		}

		serv_set_permit_deny(gc);
	}
	else if (gc->state == GAIM_DISCONNECTED) {
		if (ops != NULL && ops->disconnected != NULL)
			ops->disconnected(gc);
	}
}

//--------------------------------------------
//-> notify.c
#include "notify.h"
//static GaimNotifyUiOps *notify_ui_ops = NULL;
static GList *handles = NULL;
typedef struct
{
	GaimNotifyType type;
	void *handle;
	void *ui_handle;

} GaimNotifyInfo;

void *
gaim_notify_message(void *handle, GaimNotifyMsgType type,
					const char *title, const char *primary,
					const char *secondary, GCallback cb, void *user_data)
{
	GaimNotifyUiOps *ops;

	g_return_val_if_fail(primary != NULL, NULL);

	ops = gaim_get_notify_ui_ops();

	if (ops != NULL && ops->notify_message != NULL) {
		GaimNotifyInfo *info;

		info            = g_new0(GaimNotifyInfo, 1);
		info->type      = GAIM_NOTIFY_MESSAGE;
		info->handle    = handle;
		info->ui_handle = ops->notify_message(type, title, primary,
											  secondary, cb, user_data);

		handles = g_list_append(handles, info);

		return info->ui_handle;
	}

	return NULL;
}

void *
gaim_notify_emails(void *handle, size_t count, gboolean detailed,
				   const char **subjects, const char **froms,
				   const char **tos, const char **urls,
				   GCallback cb, void *user_data)
{
	GaimNotifyUiOps *ops;

	g_return_val_if_fail(count != 0, NULL);

	if (count == 1) {
		return gaim_notify_email(handle,
								 (subjects == NULL ? NULL : *subjects),
								 (froms    == NULL ? NULL : *froms),
								 (tos      == NULL ? NULL : *tos),
								 (urls     == NULL ? NULL : *urls),
								 cb, user_data);
	}

	ops = gaim_get_notify_ui_ops();

	if (ops != NULL && ops->notify_emails != NULL) {
		GaimNotifyInfo *info;

		info            = g_new0(GaimNotifyInfo, 1);
		info->type      = GAIM_NOTIFY_EMAILS;
		info->handle    = handle;
		info->ui_handle = ops->notify_emails(count, detailed, subjects,
											 froms, tos, urls, cb, user_data);

		handles = g_list_append(handles, info);

		return info->ui_handle;
	}

	return NULL;
}

void *
gaim_notify_formatted(void *handle, const char *title, const char *primary,
					  const char *secondary, const char *text,
					  GCallback cb, void *user_data)
{
	GaimNotifyUiOps *ops;

	g_return_val_if_fail(primary != NULL, NULL);

	ops = gaim_get_notify_ui_ops();

	if (ops != NULL && ops->notify_formatted != NULL) {
		GaimNotifyInfo *info;

		info            = g_new0(GaimNotifyInfo, 1);
		info->type      = GAIM_NOTIFY_FORMATTED;
		info->handle    = handle;
		info->ui_handle = ops->notify_formatted(title, primary, secondary,
												text, cb, user_data);

		handles = g_list_append(handles, info);

		return info->ui_handle;
	}

	return NULL;
}



//-----------------------------------------
// -> account.c
typedef struct
{
	GaimPrefType type;

	char *ui;

	union
	{
		int integer;
		char *string;
		gboolean bool;

	} value;

} GaimAccountSetting;
gboolean
gaim_account_get_check_mail(const GaimAccount *account)
{
	g_return_val_if_fail(account != NULL, FALSE);

	return gaim_account_get_bool(account, "check-mail", FALSE);
}

const char *
gaim_account_get_password(const GaimAccount *account)
{
	g_return_val_if_fail(account != NULL, NULL);

	return account->password;
}
const char *
gaim_account_get_buddy_icon(const GaimAccount *account)
{
	g_return_val_if_fail(account != NULL, NULL);

	return account->buddy_icon;
}


const char *
gaim_account_get_string(const GaimAccount *account, const char *name,
						const char *default_value)
{
	GaimAccountSetting *setting;

	g_return_val_if_fail(account != NULL, default_value);
	g_return_val_if_fail(name    != NULL, default_value);

	setting = g_hash_table_lookup(account->settings, name);

	if (setting == NULL)
		return default_value;

	g_return_val_if_fail(setting->type == GAIM_PREF_STRING, default_value);

	return setting->value.string;
}
const char *
gaim_account_get_username(const GaimAccount *account)
{
	g_return_val_if_fail(account != NULL, NULL);

	return account->username;
}
GaimConnection *
gaim_account_get_connection(const GaimAccount *account)
{
	g_return_val_if_fail(account != NULL, NULL);

	return account->gc;
}
int
gaim_account_get_int(const GaimAccount *account, const char *name,
					 int default_value)
{
	GaimAccountSetting *setting;

	g_return_val_if_fail(account != NULL, default_value);
	g_return_val_if_fail(name    != NULL, default_value);

	setting = g_hash_table_lookup(account->settings, name);

	if (setting == NULL)
		return default_value;

	g_return_val_if_fail(setting->type == GAIM_PREF_INT, default_value);

	return setting->value.integer;
}



//-----------------------------------------
//->log.c
GList *log_conversations = NULL;
static FILE *open_gaim_log_file(const char *name, int *flag)
{
	char *buf;
	char *buf2;
	char log_all_file[256];
	struct stat st;
	FILE *fd;
#ifndef _WIN32
	int res;
#endif
	gchar *gaim_dir;

	buf = g_malloc(BUF_LONG);
	buf2 = g_malloc(BUF_LONG);
	gaim_dir = gaim_user_dir();

	/*  Dont log yourself */
	strncpy(log_all_file, gaim_dir, 256);

#ifndef _WIN32
	stat(log_all_file, &st);
	if (!S_ISDIR(st.st_mode))
		unlink(log_all_file);

	fd = fopen(log_all_file, "r");

	if (!fd) {
		res = mkdir(log_all_file, S_IRUSR | S_IWUSR | S_IXUSR);
		if (res < 0) {
			g_snprintf(buf, BUF_LONG, _("Unable to make directory %s for logging"),
				   log_all_file);
			gaim_notify_error(NULL, NULL, buf, NULL);
			g_free(buf);
			g_free(buf2);
			return NULL;
		}
	} else
		fclose(fd);

	g_snprintf(log_all_file, 256, "%s" G_DIR_SEPARATOR_S "logs", gaim_dir);

	if (stat(log_all_file, &st) < 0)
		*flag = 1;
	if (!S_ISDIR(st.st_mode))
		unlink(log_all_file);

	fd = fopen(log_all_file, "r");
	if (!fd) {
		res = mkdir(log_all_file, S_IRUSR | S_IWUSR | S_IXUSR);
		if (res < 0) {
			g_snprintf(buf, BUF_LONG, _("Unable to make directory %s for logging"),
				   log_all_file);
			gaim_notify_error(NULL, NULL, buf, NULL);
			g_free(buf);
			g_free(buf2);
			return NULL;
		}
	} else
		fclose(fd);
#else /* _WIN32 */
	g_snprintf(log_all_file, 256, "%s" G_DIR_SEPARATOR_S "logs", gaim_dir);

	if( _mkdir(log_all_file) < 0 && errno != EEXIST ) {
	  g_snprintf(buf, BUF_LONG, _("Unable to make directory %s for logging"), log_all_file);
	  gaim_notify_error(NULL, NULL, buf, NULL);
	  g_free(buf);
	  g_free(buf2);
	  return NULL;
	}
#endif

	g_snprintf(log_all_file, 256, "%s" G_DIR_SEPARATOR_S "logs" G_DIR_SEPARATOR_S "%s", gaim_dir, name);
	if (stat(log_all_file, &st) < 0)
		*flag = 1;

	gaim_debug(GAIM_DEBUG_INFO, "log", "Logging to: \"%s\"\n", log_all_file);

	fd = fopen(log_all_file, "a");

	g_free(buf);
	g_free(buf2);
	return fd;
}
struct log_conversation *find_log_info(const char *name)
{
	char *pname = g_malloc(BUF_LEN);
	GList *lc = log_conversations;
	struct log_conversation *l;


	strcpy(pname, normalize(name));

	while (lc) {
		l = (struct log_conversation *)lc->data;
		/*/////
		if (!gaim_utf8_strcasecmp(pname, normalize(l->name))) {
			g_free(pname);
			return l;
			}*/ //syntax error... why?
		lc = lc->next;
	}
	g_free(pname);
	return NULL;
}
FILE *open_log_file(const char *name, int is_chat)
{
  //	struct stat st;
	char realname[256];
	struct log_conversation *l;
	FILE *fd;
	int flag = 0;

	if (((is_chat == 2) && !gaim_prefs_get_bool("/gaim/gtk/logging/individual_logs"))
		|| ((is_chat == 1) && !gaim_prefs_get_bool("/gaim/gtk/logging/log_chats"))
		|| ((is_chat == 0) && !gaim_prefs_get_bool("/gaim/gtk/logging/log_ims"))) {

		l = find_log_info(name);
		if (!l)
			return NULL;

		///		if (stat(l->filename, &st) < 0)
		/// flag = 1;

		/// fd = fopen(l->filename, "a");

		if (flag) {	/* is a new file */
			if (gaim_prefs_get_bool("/gaim/gtk/logging/strip_html")) {
				fprintf(fd, _("IM Sessions with %s\n"), name);
			} else {
				fprintf(fd, "<HTML><HEAD><TITLE>");
				fprintf(fd, _("IM Sessions with %s"), name);
				fprintf(fd, "</TITLE></HEAD><BODY BGCOLOR=\"#ffffff\">\n");
			}
		}

		return fd;
	}

	g_snprintf(realname, sizeof(realname), "%s.log", normalize(name));
	fd = open_gaim_log_file(realname, &flag);

	if (fd && flag) {	/* is a new file */
		if (gaim_prefs_get_bool("/gaim/gtk/logging/strip_html")) {
			fprintf(fd, _("IM Sessions with %s\n"), name);
		} else {
			fprintf(fd, "<HTML><HEAD><TITLE>");
			fprintf(fd, _("IM Sessions with %s"), name);
			fprintf(fd, "</TITLE></HEAD><BODY BGCOLOR=\"#ffffff\">\n");
		}
	}

	return fd;
}

//-------------------------------------------------------
// -> request.c
typedef struct
{
	GaimRequestType type;
	void *handle;
	void *ui_handle;

} GaimRequestInfo;

void *
gaim_request_input(void *handle, const char *title, const char *primary,
				   const char *secondary, const char *default_value,
				   gboolean multiline, gboolean masked,
				   const char *ok_text, GCallback ok_cb,
				   const char *cancel_text, GCallback cancel_cb,
				   void *user_data)
{
	GaimRequestUiOps *ops;

	g_return_val_if_fail(primary != NULL, NULL);
	g_return_val_if_fail(ok_text != NULL, NULL);
	g_return_val_if_fail(ok_cb   != NULL, NULL);

	ops = gaim_get_request_ui_ops();

	if (ops != NULL && ops->request_input != NULL) {
		GaimRequestInfo *info;

		info            = g_new0(GaimRequestInfo, 1);
		info->type      = GAIM_REQUEST_INPUT;
		info->handle    = handle;
		info->ui_handle = ops->request_input(title, primary, secondary,
											 default_value,
											 multiline, masked,
											 ok_text, ok_cb,
											 cancel_text, cancel_cb,
											 user_data);

		handles = g_list_append(handles, info);

		return info->ui_handle;
	}

	return NULL;
}

void *
gaim_request_action(void *handle, const char *title, const char *primary,
					const char *secondary, unsigned int default_action,
					void *user_data, size_t action_count, ...)
{
	void *ui_handle;
	va_list args;

	g_return_val_if_fail(primary != NULL,  NULL);
	g_return_val_if_fail(action_count > 0, NULL);

	va_start(args, action_count);
	ui_handle = gaim_request_action_varg(handle, title, primary, secondary,
										 default_action, user_data,
										 action_count, args);
	va_end(args);

	return ui_handle;
}



//-------------------------------------------
// -> buddyicon.c
static GHashTable *account_cache = NULL;
void
gaim_buddy_icons_set_for_user(GaimAccount *account, const char *username,
							  void *icon_data, size_t icon_len)
{
	g_return_if_fail(account  != NULL);
	g_return_if_fail(username != NULL);

	gaim_buddy_icon_new(account, username, icon_data, icon_len);
}
GaimBuddyIcon *
gaim_buddy_icon_new(GaimAccount *account, const char *username,
					void *icon_data, size_t icon_len)
{
	GaimBuddyIcon *icon;

	g_return_val_if_fail(account   != NULL, NULL);
	g_return_val_if_fail(username  != NULL, NULL);
	g_return_val_if_fail(icon_data != NULL, NULL);
	g_return_val_if_fail(icon_len  > 0,    NULL);

	icon = gaim_buddy_icons_find(account, username);

	if (icon == NULL)
	{
		GHashTable *icon_cache;

		icon = g_new0(GaimBuddyIcon, 1);

		gaim_buddy_icon_set_account(icon,  account);
		gaim_buddy_icon_set_username(icon, username);

		icon_cache = g_hash_table_lookup(account_cache, account);

		if (icon_cache == NULL)
		{
			icon_cache = g_hash_table_new(g_str_hash, g_str_equal);

			g_hash_table_insert(account_cache, account, icon_cache);
		}

		g_hash_table_insert(icon_cache,
							(char *)gaim_buddy_icon_get_username(icon), icon);
	}

	gaim_buddy_icon_set_data(icon, icon_data, icon_len);

	gaim_buddy_icon_ref(icon);

	return icon;
}
