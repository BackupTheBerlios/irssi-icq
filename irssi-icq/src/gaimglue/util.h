/**
 * @file util.h Utility Functions
 * @ingroup core
 *
 * gaim
 *
 * Copyright (C) 2002-2003, Christian Hammond <chipx86@gnupdate.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * @todo Rename the functions so that they live somewhere in the gaim
 *       namespace.
 */
#ifndef _GAIM_UTIL_H_
#define _GAIM_UTIL_H_

#include <stdio.h>

#include "account.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Normalizes a string, so that it is suitable for comparison.
 *
 * The returned string will point to a static buffer, so if the
 * string is intended to be kept long-term, you <i>must</i>
 * g_strdup() it. Also, calling normalize() twice in the same line
 * will lead to problems.
 *
 * @param str The string to normalize.
 *
 * @return A pointer to the normalized version stored in a static buffer.
 */
char *normalize(const char *str);

/**
 * Converts a string to its base-64 equivalent.
 *
 * @param buf The data to convert.
 * @param len The length of the data.
 *
 * @return The base-64 version of @a str.
 *
 * @see frombase64()
 */
char *tobase64(const unsigned char *buf, size_t len);

/**
 * Converts a string back from its base-64 equivalent.
 *
 * @param str     The string to convert back.
 * @param ret_str The returned, non-base-64 string.
 * @param ret_len The returned string length.
 *
 * @see tobase64()
 */
void frombase64(const char *str, char **ret_str, int *ret_len);

/**
 * Converts a string to its base-16 equivalent.
 *
 * @param str The string to convert.
 * @param len The length of the string.
 *
 * @return The base-16 string.
 *
 * @see frombase16()
 */
unsigned char *tobase16(const unsigned char *str, int len);

/**
 * Converts a string back from its base-16 equivalent.
 *
 * @param str     The string to convert back.
 * @param ret_str The returned, non-base-16 string.
 *
 * @return The length of the returned string.
 *
 * @see tobase16()
 */
int frombase16(const char *str, unsigned char **ret_str);

/**
 * Waits for all child processes to terminate.
 */
void clean_pid(void);

/**
 * Returns the current local time in hour:minute:second form.
 *
 * The returned string is stored in a static buffer, so the result
 * should be g_strdup()'d if it's intended to be used for long.
 *
 * @return The current local time.
 *
 * @see full_date()
 */
char *date(void);

/**
 * Adds the necessary HTML code to turn URIs into HTML links in a string.
 *
 * @param str The string to linkify.
 *
 * @return The linkified text.
 */
char *linkify_text(const char *str);

/**
 * Converts seconds into a human-readable form.
 *
 * @param sec The seconds.
 *
 * @return A human-readable form, containing days, hours, minutes, and
 *         seconds.
 */
char *sec_to_text(guint sec);

/**
 * Returns the date and time in human-readable form.
 *
 * The returned string is stored in a static buffer, so the result
 * should be g_strdup()'d if it's intended to be used for long.
 *
 * @return The date and time in human-readable form.
 *
 * @see date()
 */
char *full_date(void);

/**
 * Looks for %n, %d, or %t in a string, and replaces them with the
 * specified name, date, and time, respectively.
 *
 * The returned string is stored in a static buffer, so the result
 * should be g_strdup()'d if it's intended to be used for long.
 *
 * @param str  The string that may contain the special variables.
 * @param name The sender name.
 *
 * @return A new string where the special variables are expanded.
 */
char *away_subs(const char *str, const char *name);

/**`
 * Returns the user's home directory.
 *
 * @return The user's home directory.
 *
 * @see gaim_user_dir()
 */
const gchar *gaim_home_dir(void);

/**
 * Returns the gaim settings directory in the user's home directory.
 * 
 * @return The gaim settings directory.
 *
 * @see gaim_home_dir()
 */
char *gaim_user_dir(void);

/**
 * Copies a string and replaces all HTML linebreaks with newline characters.
 *
 * @param dest     The destination string.
 * @param src      The source string.
 * @param dest_len The destination string length.
 *
 * @see strncpy_withhtml()
 * @see strdup_withhtml()
 */
void strncpy_nohtml(gchar *dest, const gchar *src, size_t dest_len);

/**
 * Copies a string and replaces all newline characters with HTML linebreaks.
 *
 * @param dest     The destination string.
 * @param src      The source string.
 * @param dest_len The destination string length.
 *
 * @see strncpy_nohtml()
 * @see strdup_withhtml()
 */
void strncpy_withhtml(gchar *dest, const gchar *src, size_t dest_len);

/**
 * Duplicates a string and replaces all newline characters from the
 * source string with HTML linebreaks.
 *
 * @param src The source string.
 *
 * @return The new string.
 *
 * @see strncpy_nohtml()
 * @see strncpy_withhtml()
 */
gchar *strdup_withhtml(const gchar *src);

/**
 * Ensures that all linefeeds have a matching carriage return.
 *
 * @param str The source string.
 *
 * @return The string with carriage returns.
 */
char *add_cr(const char *str);

/**
 * Strips all linefeeds from a string.
 *
 * @param str The string to strip linefeeds from.
 */
void strip_linefeed(char *str);

/**
 * Builds a time_t from the supplied information.
 *
 * @param year  The year.
 * @param month The month.
 * @param day   The day.
 * @param hour  The hour.
 * @param min   The minute.
 * @param sec   The second.
 *
 * @return A time_t.
 */
time_t get_time(int year, int month, int day,
				int hour, int min, int sec);

/**
 * Creates a temporary file and returns a file pointer to it.
 *
 * This is like mkstemp(), but returns a file pointer and uses a
 * pre-set template. It uses the semantics of tempnam() for the
 * directory to use and allocates the space for the file path.
 *
 * The caller is responsible for closing the file and removing it when
 * done, as well as freeing the space pointed to by @a path with
 * g_free().
 *
 * @param path The returned path to the temp file.
 * 
 * @return A file pointer to the temporary file, or @c NULL on failure.
 */
FILE *gaim_mkstemp(gchar **path);

/**
 * Checks if the given program name is valid and executable.
 *
 * @parm program The file name of the application.
 *
 * @return True if the program is runable.
 */
gboolean program_is_valid(const char *program);

/**
 * Attempts to convert a string to UTF-8 from an unknown encoding.
 *
 * This function checks the locale and tries sane defaults.
 *
 * @param str The source string.
 *
 * @return The UTF-8 string, or @c NULL if it could not be converted.
 */
char *gaim_try_conv_to_utf8(const char *str);

/**
 * Returns the IP address from a socket file descriptor.
 *
 * @param fd The socket file descriptor.
 *
 * @return The IP address, or @c NULL on error.
 */
char *gaim_getip_from_fd(int fd);

/**
 * Compares two UTF-8 strings.
 *
 * @param a The first string.
 * @param b The second string.
 *
 * @return -1 if @a is less than @a b.
 *          0 if @a is equal to @a b.
 *          1 if @a is greater than @a b.
 */
gint gaim_utf8_strcasecmp(const gchar *a, const gchar *b);

/**
 * Given a string, this replaces one substring with another
 * and returns a newly allocated string.
 *
 * @param string The string from which to replace stuff.
 * @param delimiter The substring you want replaced.
 * @param replacement The substring you want inserted in place 
 *        of the delimiting substring.
 */
gchar *gaim_strreplace(const gchar *string, const gchar *delimiter,
					   const gchar *replacement);

/**
 * This is like strstr, except that it ignores ASCII case in
 * searching for the substring.
 *
 * @param haystack The string to search in.
 * @param needle   The substring to find.
 * @return the location of the substring if found, or NULL if not
 */
const char *gaim_strcasestr(const char *haystack, const char *needle);

/**
 * Returns a string representing a filesize in the appropriate
 * units (MB, KB, GB, etc.)
 *
 * @param size The size
 */
char *gaim_get_size_string(size_t size);

/**
 * Finds a HTML tag matching the given name, locating its start
 * and end, and storing its attributes in a GData hash table.
 * The names of the attributes are lower-cased in the hash table,
 * and the name of the tag is case insensitive.
 *
 * @param needle	the name of the tag
 * @param haystack	the null-delimited string to search in
 * @param start		a pointer to the start of the tag if found
 * @param end		a pointer to the end of the tag if found
 * @param attributes	the attributes, if the tag was found
 * @return TRUE if the tag was found
 */
gboolean gaim_markup_find_tag(const char *needle, const char *haystack, const char **start, const char **end, GData **attributes);

#ifdef __cplusplus
}
#endif

#endif /* _GAIM_UTIL_H_ */
