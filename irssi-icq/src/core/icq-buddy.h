void add_buddy(const char *uin, const char *alias);
void destroy_buddy_list(void);
const char *buddy_getalias(const char *uin);
const char *buddy_getuin(const char *alias);
void read_buddy_file(void);
void buddy_forall(GFunc f, gpointer user_data);
char *read_conf_option(const char *opt);
int buddy_getmode(const char *uin);
int buddy_setmode(const char *uin, int mode);
guint32 buddy_getip(const char *uin);
int buddy_setip(const char *uin, guint32 mode);
#define OFFLINE 1243	/* some magic number, not used by aim */
const char *modestring(int status);
extern const char *away_modes[];
int parse_away_mode(const char *status);
void icq_save_buddy_file(void);
void icq_reread_buddy_file(void);

