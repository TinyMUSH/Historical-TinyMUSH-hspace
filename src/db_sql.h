/* db_sql.h */

#include "copyright.h"

/*
 * If we are already connected to the database, do nothing.
 * Otherwise, attempt to connect: return a socket fd if successful,
 * return -1 if failed, stamp a message to the logfile.
 * Select a database. On failure, write to the logfile, close the socket,
 * and return -1.
 * Save the database socket fd as mudstate.sql_socket (which is initialized
 * to -1 at start time).
 *
 */
int NDECL(sql_init);

/*
 * Send a query string to the database and obtain a result string (we
 * pass an lbuf and a pointer to the appropriate place in it),
 * with the rows separated by a delimiter and the fields separated
 * by another delimiter. If we encounter an error, set the result
 * string to #-1.
 * If buff is NULL, we are in interactive mode and we simply notify
 * the player of results, rather than writing into the result string.
 * On success, return 0. On falure, return -1.
 */
int FDECL(sql_query, (dbref, char *, char *, char **, char, char));

/*
 * Shut down the database connection.
 */
void NDECL(sql_shutdown);
