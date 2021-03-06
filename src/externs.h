
/* externs.h - Prototypes for externs not defined elsewhere */
/* $Id$ */

#include "copyright.h"

#ifndef __EXTERNS__H
#define	__EXTERNS__H

#include "autoconf.h"
#include "db.h"
#include "mudconf.h"
#include "regexp.h"

#define INLINE inline

#define ToUpper(C)	(((C) >= 'a' && (C) <= 'z')? (C) - 'a' + 'A': (C))
#define ToLower(C)	(((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))
#define safe_atoi(s)	((s == NULL) ? 0 : atoi(s))

/* From regexp.c (extract from Henry Spencer's package) */

extern regexp *FDECL(regcomp, (char *));
extern int FDECL(regexec, (register regexp *, register char *));
extern void FDECL(regerror, (char *));
extern char regexp_errbuf[];

/* From conf.c */
extern int	FDECL(cf_modify_bits, (int *, char *, long, dbref, char *));

/* From mail.c*/
extern int	FDECL(load_mail, (FILE *));
extern int	FDECL(dump_mail, (FILE *));
extern void	NDECL(mail_init);
extern struct mail *	FDECL(mail_fetch, (dbref, int));

/* From udb_achunk.c */
extern int	NDECL(dddb_close);
extern int	FDECL(dddb_setfile, (char *));
extern int	NDECL(dddb_init);

/* From netcommon.c */
extern void	FDECL(make_ulist, (dbref, char *, char **));
extern int	FDECL(fetch_idle, (dbref));
extern int	FDECL(fetch_connect, (dbref));
extern void	VDECL(raw_broadcast, (int, char *, ...));

/* From cque.c */
extern int	FDECL(nfy_que, (dbref, dbref, int, int, int));
extern int	FDECL(halt_que, (dbref, dbref));
extern void	FDECL(wait_que, (dbref, dbref, int, dbref, int, char *, char *[],
			int, char *[]));
extern int	NDECL(que_next);
extern int	FDECL(do_top, (int ncmds));
extern void	NDECL(recover_queue_deposits);

/* From eval.c */
extern void	NDECL(tcache_init);
extern char *	FDECL(parse_to, (char **, char, int));
extern char *	FDECL(parse_arglist, (dbref, dbref, char *, char, int,
			char *[], int, char*[], int));
extern int	FDECL(get_gender, (dbref));
extern void	FDECL(exec, (char *, char **, int, dbref, dbref, int, char **,
			char *[], int));
extern void	FDECL(save_global_regs, (const char *, char *[], int []));
extern void	FDECL(restore_global_regs, (const char *, char *[], int []));

/* From game.c */
#define	notify(p,m)			notify_check(p,p,m, \
						MSG_PUP_ALWAYS|MSG_ME_ALL|MSG_F_DOWN)
#define notify_html(p,m)                notify_check(p,p,m, \
                                              MSG_PUP_ALWAYS|MSG_ME_ALL|MSG_F_DOWN|MSG_HTML)
#define	notify_quiet(p,m)		notify_check(p,p,m, \
						MSG_PUP_ALWAYS|MSG_ME)
#define	notify_with_cause(p,c,m)	notify_check(p,c,m, \
						MSG_PUP_ALWAYS|MSG_ME_ALL|MSG_F_DOWN)
#define notify_with_cause_html(p,c,m)   notify_check(p,c,m, \
                                              MSG_PUP_ALWAYS|MSG_ME_ALL|MSG_F_DOWN|MSG_HTML)
#define	notify_quiet_with_cause(p,c,m)	notify_check(p,c,m, \
						MSG_PUP_ALWAYS|MSG_ME)
#define	notify_puppet(p,c,m)		notify_check(p,c,m, \
						MSG_ME_ALL|MSG_F_DOWN)
#define	notify_quiet_puppet(p,c,m)	notify_check(p,c,m, \
						MSG_ME)
#define	notify_all(p,c,m)		notify_check(p,c,m, \
						MSG_ME_ALL|MSG_NBR_EXITS|MSG_F_UP|MSG_F_CONTENTS)
#define	notify_all_from_inside(p,c,m)	notify_check(p,c,m, \
						MSG_ME_ALL|MSG_NBR_EXITS_A|MSG_F_UP|MSG_F_CONTENTS|MSG_S_INSIDE)
#define notify_all_from_inside_html(p,c,m)      notify_check(p,c,m, \
                                              MSG_ME_ALL|MSG_NBR_EXITS_A|MSG_F_UP|MSG_F_CONTENTS|MSG_S_INSIDE|MSG_HTML) 
#define	notify_all_from_outside(p,c,m)	notify_check(p,c,m, \
						MSG_ME_ALL|MSG_NBR_EXITS|MSG_F_UP|MSG_F_CONTENTS|MSG_S_OUTSIDE)
extern void	FDECL(notify_except, (dbref, dbref, dbref,
			const char *));
extern void	FDECL(notify_except2, (dbref, dbref, dbref, dbref,
			 const char *));
extern int	FDECL(check_filter, (dbref, dbref, int,
			const char *));
extern void	FDECL(notify_check, (dbref, dbref, const char *, int));
extern int	FDECL(Hearer, (dbref));
extern void	NDECL(report);
extern int	FDECL(atr_match, (dbref, dbref, char, char *, char *, int));
extern int	FDECL(list_check, (dbref, dbref, char, char *, char *, int, int *));
extern void	FDECL(html_escape, (const char *src, char *dest, char **destp));
	
/* From help.c */
extern int	FDECL(helpindex_read, (HASHTAB *, char *));
extern void	FDECL(helpindex_load, (dbref));
extern void	NDECL(helpindex_init);

/* From htab.c */
extern int	FDECL(cf_ntab_access, (int *, char *, long, dbref, char *));

/* From log.c */
extern FILE *	mainlog_fp;
extern char *	FDECL(strip_ansi, (const char *));
extern char *	FDECL(normal_to_white, (const char *));
extern int	FDECL(start_log, (const char *, const char *, int));
extern void	NDECL(end_log);
extern void	FDECL(log_perror, (const char *, const char *,const char *,
			const char *));
extern void	FDECL(log_string, (char *));
extern void	FDECL(log_text, (char *));
extern void	FDECL(log_number, (int));
extern void	FDECL(log_name, (dbref));
extern void	FDECL(log_name_and_loc, (dbref));
extern char *	FDECL(OBJTYP, (dbref));
extern void	FDECL(log_type_and_name, (dbref));

/* From look.c */
extern void	FDECL(look_in, (dbref,dbref, int));
extern void	FDECL(show_vrml_url, (dbref, dbref));

/* From move.c */
extern void	FDECL(move_object, (dbref, dbref));
extern void	FDECL(move_via_generic, (dbref, dbref, dbref, int));
extern void	FDECL(move_via_exit, (dbref, dbref, dbref, dbref, int));
extern int	FDECL(move_via_teleport, (dbref, dbref, dbref, int));
extern void	FDECL(move_exit, (dbref, dbref, int, const char *, int));
extern void	FDECL(do_enter_internal, (dbref, dbref, int));

/* From object.c */
extern dbref	NDECL(start_home);
extern dbref	NDECL(default_home);
extern int	FDECL(can_set_home, (dbref, dbref, dbref));
extern dbref	FDECL(new_home, (dbref));
extern dbref	FDECL(clone_home, (dbref, dbref));
extern void	FDECL(divest_object, (dbref));
extern dbref	FDECL(create_obj, (dbref, int, char *, int));
extern void	FDECL(destroy_obj, (dbref, dbref));
extern void	FDECL(empty_obj, (dbref));

/* From player.c */
extern void	FDECL(record_login, (dbref, int, char *, char *, char *));
extern int	FDECL(check_pass, (dbref, const char *));
extern dbref	FDECL(connect_player, (char *, char *, char *, char *));
extern dbref	FDECL(create_player, (char *, char *, dbref, int, int));
extern int	FDECL(add_player_name, (dbref, char *));
extern int	FDECL(delete_player_name, (dbref, char *));
extern dbref	FDECL(lookup_player, (dbref, char *, int));
extern void	NDECL(load_player_names);
extern void	FDECL(badname_add, (char *));
extern void	FDECL(badname_remove, (char *));
extern int	FDECL(badname_check, (char *));
extern void	FDECL(badname_list, (dbref, const char *));

/* From predicates.c */
extern char *	VDECL(tprintf, (char *, ...));
extern void	VDECL(safe_tprintf_str, (char *, char **, char *, ...));
extern dbref	FDECL(insert_first, (dbref, dbref));
extern dbref	FDECL(remove_first, (dbref, dbref));
extern dbref	FDECL(reverse_list, (dbref));
extern int	FDECL(member, (dbref, dbref));
extern int	FDECL(is_integer, (char *));
extern int	FDECL(is_number, (char *));
extern int	FDECL(could_doit, (dbref, dbref, int));
extern int	FDECL(can_see, (dbref, dbref, int));
extern void	FDECL(add_quota, (dbref, int, int));
extern int	FDECL(canpayfees, (dbref, dbref, int, int, int));
extern int	FDECL(payfees, (dbref, int, int, int));
extern void	FDECL(giveto, (dbref,int));
extern int	FDECL(payfor, (dbref,int));
extern int	FDECL(ok_name, (const char *));
extern int	FDECL(ok_player_name, (const char *));
extern int	FDECL(ok_attr_name, (const char *));
extern int	FDECL(ok_password, (const char *, dbref));
extern void	FDECL(handle_ears, (dbref, int, int));
extern dbref	FDECL(match_possessed, (dbref, dbref, char *, dbref, int));
extern void	FDECL(parse_range, (char **, dbref *, dbref *));
extern int	FDECL(parse_thing_slash, (dbref, char *, char **, dbref *));
extern int	FDECL(get_obj_and_lock, (dbref, char *, dbref *, ATTR **,
			char *, char **));
extern dbref	FDECL(where_is, (dbref));
extern dbref	FDECL(where_room, (dbref));
extern int	FDECL(locatable, (dbref, dbref, dbref));
extern int	FDECL(nearby, (dbref, dbref));
extern int	FDECL(exit_visible, (dbref, dbref, int));
extern void	FDECL(did_it, (dbref, dbref, int, const char *, int,
			const char *, int, char *[], int));
extern void	FDECL(list_bufstats, (dbref));
extern void	FDECL(list_buftrace, (dbref));

/* From set.c */
extern int	FDECL(parse_attrib, (dbref, char *, dbref *, int *));
extern int	FDECL(parse_attrib_wild, (dbref, char *, dbref *, int,
			int, int));
extern void	FDECL(edit_string, (char *, char **, char *, char *));
extern dbref	FDECL(match_controlled, (dbref, const char *));

/* From stringutil.c */
extern char *	FDECL(munge_space, (char *));
extern char *	FDECL(trim_spaces, (char *));
extern char *	FDECL(grabto, (char **, char));
extern int	FDECL(string_compare, (const char *, const char *));
extern int	FDECL(string_prefix, (const char *, const char *));
extern const char *	FDECL(string_match, (const char * ,const char *));
extern char *	FDECL(dollar_to_space, (const char *));
extern char *	FDECL(replace_string, (const char *, const char *,
			const char *));
extern char *	FDECL(replace_string_ansi, (const char *, const char *,
			const char *));
extern char *	FDECL(skip_space, (const char *));
extern char *	FDECL(seek_char, (const char *, char));
extern int	FDECL(minmatch, (char *, char *, int));
extern INLINE int FDECL(safe_copy_str, (char *, char *, char **, int));
extern int	FDECL(safe_copy_long_str, (char *, char *, char **, int));
extern INLINE void FDECL(safe_copy_known_str, (char *, int, char *, char **,
					       int));
extern int	FDECL(matches_exit_from_list, (char *, char *));
extern char *	FDECL(translate_string, (const char *, int));
extern int	FDECL(ltos, (char *, long));
extern INLINE void FDECL(safe_ltos, (char *, char **, long));
/* From boolexp.c */
extern int	FDECL(eval_boolexp, (dbref, dbref, dbref, BOOLEXP *));
extern BOOLEXP *FDECL(parse_boolexp, (dbref,const char *, int));
extern int	FDECL(eval_boolexp_atr, (dbref, dbref, dbref, char *));

/* From functions.c */
extern int	FDECL(xlate, (char *));

/* From unparse.c */
extern char *	FDECL(unparse_boolexp, (dbref, BOOLEXP *));
extern char *	FDECL(unparse_boolexp_quiet, (dbref, BOOLEXP *));
extern char *	FDECL(unparse_boolexp_decompile, (dbref, BOOLEXP *));
extern char *	FDECL(unparse_boolexp_function, (dbref, BOOLEXP *));

/* From walkdb.c */
extern int	FDECL(chown_all, (dbref, dbref, dbref, int));
extern void	NDECL(olist_push);
extern void	NDECL(olist_pop);
extern void	FDECL(olist_add, (dbref));
extern dbref	NDECL(olist_first);
extern dbref	NDECL(olist_next);

/* From wild.c */
extern int	FDECL(wild, (char *, char *, char *[], int));
extern int	FDECL(wild_match, (char *, char *));
extern int	FDECL(quick_wild, (char *, char *));

/* From compress.c */
extern const char *	FDECL(uncompress, (const char *, int));
extern const char *	FDECL(compress, (const char *, int));
extern char *	FDECL(uncompress_str, (char *, const char *, int));

/* From command.c */
extern int	FDECL(check_access, (dbref, int));
extern void	NDECL(set_prefix_cmds);

#define	Protect(f) (cmdp->perms & f)

#define Invalid_Objtype(x) \
((Protect(CA_LOCATION) && !Has_location(x)) || \
 (Protect(CA_CONTENTS) && !Has_contents(x)) || \
 (Protect(CA_PLAYER) && (Typeof(x) != TYPE_PLAYER)))

/* from db.c */
extern int	FDECL(Commer, (dbref));
extern void	FDECL(s_Pass, (dbref, const char *));
extern INLINE void	FDECL(s_Name, (dbref, char *));
extern INLINE char *	FDECL(Name, (dbref));
extern INLINE char *	FDECL(PureName, (dbref));
extern INLINE void	FDECL(safe_name, (dbref, char *, char **));
extern int	FDECL(fwdlist_load, (FWDLIST *, dbref, char *));
extern void	FDECL(fwdlist_set, (dbref, FWDLIST *));
extern void	FDECL(fwdlist_clr, (dbref));
extern int	FDECL(fwdlist_rewrite, (FWDLIST *, char *));
#ifdef STANDALONE
extern FWDLIST *FDECL(fwdlist_get, (dbref));
#else
#define fwdlist_get(x) ((FWDLIST *) nhashfind((x), &mudstate.fwdlist_htab))
#endif
extern void	NDECL(init_min_db);
extern void	NDECL(atr_push);
extern void	NDECL(atr_pop);
extern int	FDECL(atr_head, (dbref, char **));
extern int	FDECL(atr_next, (char **));
extern int	FDECL(init_gdbm_db, (char *));
extern void	FDECL(atr_cpy, (dbref, dbref, dbref));
extern void	FDECL(atr_chown, (dbref));
extern void	FDECL(atr_clr, (dbref, int));
extern void	FDECL(atr_add_raw, (dbref, int, char *));
extern void	FDECL(atr_add, (dbref, int, char *, dbref, int));
extern void	FDECL(atr_set_owner, (dbref, int, dbref));
extern void	FDECL(atr_set_flags, (dbref, int, int));
extern char *	FDECL(atr_get_raw, (dbref, int));
extern char *	FDECL(atr_get, (dbref, int, dbref *, int *, int *));
extern char *	FDECL(atr_pget, (dbref, int, dbref *, int *, int *));
extern char *	FDECL(atr_get_str, (char *, dbref, int, dbref *, int *,
				    int *));
extern char *	FDECL(atr_pget_str, (char *, dbref, int, dbref *, int *,
				     int *));
extern int	FDECL(atr_get_info, (dbref, int, dbref *, int *));
extern int	FDECL(atr_pget_info, (dbref, int, dbref *, int *));
extern void	FDECL(atr_free, (dbref));
extern int	FDECL(check_zone, (dbref, dbref));
extern int	FDECL(check_zone_for_player, (dbref, dbref));
extern void	FDECL(toast_player, (dbref));

/* Command handler keys */

#define	ATTRIB_ACCESS	1	/* Change access to attribute */
#define	ATTRIB_RENAME	2	/* Rename attribute */
#define	ATTRIB_DELETE	4	/* Delete attribute */
#define ATTRIB_INFO	8	/* Info (number, flags) about attribute */
#define	BOOT_QUIET	1	/* Inhibit boot message to victim */
#define	BOOT_PORT	2	/* Boot by port number */
#define CBOOT_QUIET	1	/* No boot message, just has left */
#define CEMIT_NOHEADER  1	/* Channel emit without header */
#define CHANNEL_SET	1	/* Set channel flag */
#define CHANNEL_CHARGE	2	/* Set channel charge */
#define CHANNEL_DESC	4	/* Set channel desc */
#define CHANNEL_LOCK	8	/* Set channel lock */
#define CHANNEL_OWNER	16	/* Set channel owner */
#define CHANNEL_JOIN	32	/* Channel lock: join */
#define CHANNEL_TRANS	64	/* Channel lock: transmit */
#define CHANNEL_RECV	128	/* Channel lock: receive */
#define	CHOWN_ONE	1	/* item = new_owner */
#define	CHOWN_ALL	2	/* old_owner = new_owner */
#define CHOWN_NOSTRIP	4	/* Don't strip (most) flags from object */
#define CHZONE_NOSTRIP	1	/* Don't strip (most) flags from object */
#define CLIST_FULL	1	/* Full listing of channels */
#define	CLONE_LOCATION	0	/* Create cloned object in my location */
#define	CLONE_INHERIT	1	/* Keep INHERIT bit if set */
#define	CLONE_PRESERVE	2	/* Preserve the owner of the object */
#define	CLONE_INVENTORY	4	/* Create cloned object in my inventory */
#define	CLONE_SET_COST	8	/* ARG2 is cost of cloned object */
#define	CLONE_PARENT	16	/* Set parent on obj instd of cloning attrs */
#define CLONE_NOSTRIP	32	/* Don't strip (most) flags from clone */
#define CWHO_ALL	1	/* Show disconnected players on channel */
#define	DBCK_FULL	1	/* Do all tests */
#define DECOMP_PRETTY	1	/* pretty-format output */
#define	DEST_ONE	1	/* object */
#define	DEST_ALL	2	/* owner */
#define	DEST_OVERRIDE	4	/* override Safe() */
#define DEST_INSTANT	8	/* instantly destroy */
#define	DIG_TELEPORT	1	/* teleport to room after @digging */
#define DOLIST_SPACE    0       /* expect spaces as delimiter */
#define DOLIST_DELIMIT  1       /* expect custom delimiter */
#define DOLIST_NOTIFY   2     /* queue a '@notify me' at the end */
#define	DOING_MESSAGE	0	/* Set my DOING message */
#define	DOING_HEADER	1	/* Set the DOING header */
#define	DOING_POLL	2	/* List DOING header */
#define	DROP_QUIET	1	/* Don't do odrop/adrop if control */
#define	DUMP_STRUCT	1	/* Dump flat structure file */
#define	DUMP_TEXT	2	/* Dump text (gdbm) file */
#define DUMP_FLATFILE	8	/* Dump to flatfile */
#define	EXAM_DEFAULT	0	/* Default */
#define	EXAM_BRIEF	1	/* Don't show attributes */
#define	EXAM_LONG	2	/* Nonowner sees public attrs too */
#define	EXAM_DEBUG	4	/* Display more info for finding db problems */
#define	EXAM_PARENT	8	/* Get attr from parent when exam obj/attr */
#define EXAM_PRETTY	16	/* Pretty-format output */
#define EXAM_PAIRS	32	/* Print paren matches in color */
#define	EXAM_OWNER	64	/* Nonowner sees just owner */
#define	FIXDB_OWNER	1	/* Fix OWNER field */
#define	FIXDB_LOC	2	/* Fix LOCATION field */
#define	FIXDB_CON	4	/* Fix CONTENTS field */
#define	FIXDB_EXITS	8	/* Fix EXITS field */
#define	FIXDB_NEXT	16	/* Fix NEXT field */
#define	FIXDB_PENNIES	32	/* Fix PENNIES field */
#define	FIXDB_NAME	64	/* Set NAME attribute */
#define	FRC_COMMAND	1	/* what=command */
#define	GET_QUIET	1	/* Don't do osucc/asucc if control */
#define	GIVE_QUIET	1	/* Inhibit give messages */
#define	GLOB_ENABLE	1	/* key to enable */
#define	GLOB_DISABLE	2	/* key to disable */
#define	HALT_ALL	1	/* halt everything */
#define HELP_RAWHELP	0x08000000	/* A high bit. Don't eval text. */
#define HOOK_BEFORE	1	/* pre-command hook */
#define HOOK_AFTER	2	/* post-command hook */
#define HOOK_PRESERVE	4	/* preserve global regs */
#define HOOK_NOPRESERVE	8	/* don't preserve global regs */
#define	KILL_KILL	1	/* gives victim insurance */
#define	KILL_SLAY	2	/* no insurance */
#define	LOOK_LOOK	1	/* list desc (and succ/fail if room) */
#define	LOOK_INVENTORY	2	/* list inventory of object */
#define	LOOK_SCORE	4	/* list score (# coins) */
#define	LOOK_OUTSIDE    8       /* look for object in container of player */
#define MAIL_STATS	1	/* Mail stats */
#define MAIL_DSTATS	2	/* More mail stats */
#define MAIL_FSTATS	3	/* Even more mail stats */
#define MAIL_DEBUG	4	/* Various debugging options */
#define MAIL_NUKE	5	/* Nuke the post office */
#define MAIL_FOLDER	6	/* Do folder stuff */
#define MAIL_LIST	7	/* List @mail by time */
#define MAIL_READ	8	/* Read @mail message */
#define MAIL_CLEAR	9	/* Clear @mail message */
#define MAIL_UNCLEAR	10	/* Unclear @mail message */
#define MAIL_PURGE	11	/* Purge cleared @mail messages */
#define MAIL_FILE	12	/* File @mail in folders */
#define MAIL_TAG	13	/* Tag @mail messages */
#define MAIL_UNTAG	14	/* Untag @mail messages */
#define MAIL_FORWARD	15	/* Forward @mail messages */
#define MAIL_SEND	16	/* Send @mail messages in progress */
#define MAIL_EDIT	17	/* Edit @mail messages in progress */
#define MAIL_URGENT	18	/* Sends a @mail message as URGENT */
#define MAIL_ALIAS	19	/* Creates an @mail alias */
#define MAIL_ALIST	20	/* Lists @mail aliases */
#define MAIL_PROOF	21	/* Proofs @mail messages in progress */
#define MAIL_ABORT	22	/* Aborts @mail messages in progress */
#define MAIL_QUICK	23	/* Sends a quick @mail message */
#define MAIL_REVIEW	24	/* Reviews @mail sent to a player */
#define MAIL_RETRACT	25	/* Retracts @mail sent to a player */
#define MAIL_CC		26	/* Carbon copy */
#define MAIL_SAFE	27	/* Defines a piece of mail as safe. */
#define MAIL_REPLY	28	/* Replies to a message. */
#define MAIL_REPLYALL	29	/* Replies to all recipients of msg */
#define MAIL_QUOTE	0x100	/* Quote back original in the reply? */

/* Note that we don't use all hex for mail flags, because the upper 3 bits
 * of the switch word gets occupied by SW_<foo>.
 */

#define MALIAS_DESC	1	/* Describes a mail alias */
#define MALIAS_CHOWN	2	/* Changes a mail alias's owner */
#define MALIAS_ADD	3	/* Adds a player to an alias */
#define MALIAS_REMOVE	4	/* Removes a player from an alias */
#define MALIAS_DELETE	5	/* Deletes a mail alias */
#define MALIAS_RENAME	6	/* Renames a mail alias */
#define MALIAS_LIST	8	/* Lists mail aliases */
#define MALIAS_STATUS	9	/* Status of mail aliases */
#define	MARK_SET	0	/* Set mark bits */
#define	MARK_CLEAR	1	/* Clear mark bits */
#define	MOTD_ALL	0	/* login message for all */
#define	MOTD_WIZ	1	/* login message for wizards */
#define	MOTD_DOWN	2	/* login message when logins disabled */
#define	MOTD_FULL	4	/* login message when too many players on */
#define	MOTD_LIST	8	/* Display current login messages */
#define	MOTD_BRIEF	16	/* Suppress motd file display for wizards */
#define	MOVE_QUIET	1	/* Dont do osucc/ofail/asucc/afail if ctrl */
#define	NFY_NFY		0	/* Notify first waiting command */
#define	NFY_NFYALL	1	/* Notify all waiting commands */
#define	NFY_DRAIN	2	/* Delete waiting commands */
#define	OPEN_LOCATION	0	/* Open exit in my location */
#define	OPEN_INVENTORY	1	/* Open exit in me */
#define	PASS_ANY	1	/* name=newpass */
#define	PASS_MINE	2	/* oldpass=newpass */
#define	PCRE_PLAYER	1	/* create new player */
#define	PCRE_ROBOT	2	/* create robot player */
#define	PEMIT_PEMIT	1	/* emit to named player */
#define	PEMIT_OEMIT	2	/* emit to all in current room except named */
#define	PEMIT_WHISPER	3	/* whisper to player in current room */
#define	PEMIT_FSAY	4	/* force controlled obj to say */
#define	PEMIT_FEMIT	5	/* force controlled obj to emit */
#define	PEMIT_FPOSE	6	/* force controlled obj to pose */
#define	PEMIT_FPOSE_NS	7	/* force controlled obj to pose w/o space */
#define	PEMIT_CONTENTS	8	/* Send to contents (additive) */
#define	PEMIT_HERE	16	/* Send to location (@femit, additive) */
#define	PEMIT_ROOM	32	/* Send to containing rm (@femit, additive) */
#define PEMIT_LIST      64      /* Send to a list */
#define PEMIT_HTML	128     /* HTML escape, and no newline */
#define	PS_BRIEF	0	/* Short PS report */
#define	PS_LONG		1	/* Long PS report */
#define	PS_SUMM		2	/* Queue counts only */
#define	PS_ALL		4	/* List entire queue */
#define	QUEUE_KICK	1	/* Process commands from queue */
#define	QUEUE_WARP	2	/* Advance or set back wait queue clock */
#define	QUOTA_SET	1	/* Set a quota */
#define	QUOTA_FIX	2	/* Repair a quota */
#define	QUOTA_TOT	4	/* Operate on total quota */
#define	QUOTA_REM	8	/* Operate on remaining quota */
#define	QUOTA_ALL	16	/* Operate on all players */
#define QUOTA_ROOM      32      /* Room quota set */
#define QUOTA_EXIT      64      /* Exit quota set */
#define QUOTA_THING     128     /* Thing quota set */
#define QUOTA_PLAYER    256     /* Player quota set */
#define	SAY_SAY		1	/* say in current room */
#define	SAY_NOSPACE	1	/* OR with xx_EMIT to get nospace form */
#define	SAY_POSE	2	/* pose in current room */
#define	SAY_POSE_NOSPC	3	/* pose w/o space in current room */
#define	SAY_PREFIX	4	/* first char indicates foratting */
#define	SAY_EMIT	5	/* emit in current room */
#define	SAY_SHOUT	8	/* shout to all logged-in players */
#define	SAY_WALLPOSE	9	/* Pose to all logged-in players */
#define	SAY_WALLEMIT	10	/* Emit to all logged-in players */
#define	SAY_WIZSHOUT	12	/* shout to all logged-in wizards */
#define	SAY_WIZPOSE	13	/* Pose to all logged-in wizards */
#define	SAY_WIZEMIT	14	/* Emit to all logged-in wizards */
#define SAY_ADMINSHOUT	15	/* Emit to all wizards or royalty */ 
#define	SAY_NOTAG	32	/* Don't put Broadcast: in front (additive) */
#define	SAY_HERE	64	/* Output to current location */
#define	SAY_ROOM	128	/* Output to containing room */
#define SAY_HTML	256     /* Don't output a newline */
#define	SET_QUIET	1	/* Don't display 'Set.' message. */
#define	SHUTDN_PANIC	1	/* Write a panic dump file */
#define	SHUTDN_COREDUMP	2	/* Produce a coredump */
#define	SRCH_SEARCH	1	/* Do a normal search */
#define	SRCH_MARK	2	/* Set mark bit for matches */
#define	SRCH_UNMARK	3	/* Clear mark bit for matches */
#define	STAT_PLAYER	0	/* Display stats for one player or tot objs */
#define	STAT_ALL	1	/* Display global stats */
#define	STAT_ME		2	/* Display stats for me */
#define	SWITCH_DEFAULT	0	/* Use the configured default for switch */
#define	SWITCH_ANY	1	/* Execute all cases that match */
#define	SWITCH_ONE	2	/* Execute only first case that matches */
#define	SWEEP_ME	1	/* Check my inventory */
#define	SWEEP_HERE	2	/* Check my location */
#define	SWEEP_COMMANDS	4	/* Check for $-commands */
#define	SWEEP_LISTEN	8	/* Check for @listen-ers */
#define	SWEEP_PLAYER	16	/* Check for players and puppets */
#define	SWEEP_CONNECT	32	/* Search for connected players/puppets */
#define	SWEEP_EXITS	64	/* Search the exits for audible flags */
#define	SWEEP_VERBOSE	256	/* Display what pattern matches */
#define TELEPORT_DEFAULT 1	/* Emit all messages */
#define TELEPORT_QUIET   2	/* Teleport in quietly */
#define TIMECHK_RESET	1	/* Reset all counters to zero */
#define TIMECHK_SCREEN	2	/* Write info to screen */
#define TIMECHK_LOG	4	/* Write info to log */
#define	TOAD_NO_CHOWN	1	/* Don't change ownership */
#define	TRIG_QUIET	1	/* Don't display 'Triggered.' message. */
#define	TWARP_QUEUE	1	/* Warp the wait and sem queues */
#define	TWARP_DUMP	2	/* Warp the dump interval */
#define	TWARP_CLEAN	4	/* Warp the cleaning interval */
#define	TWARP_IDLE	8	/* Warp the idle check interval */
/* emprty		16 */
#define TWARP_EVENTS	32	/* Warp the events checking interval */

/* Hush codes for movement messages */

#define	HUSH_ENTER	1	/* xENTER/xEFAIL */
#define	HUSH_LEAVE	2	/* xLEAVE/xLFAIL */
#define	HUSH_EXIT	4	/* xSUCC/xDROP/xFAIL from exits */

/* Evaluation directives */

#define	EV_FIGNORE	0x00000000	/* Don't look for func if () found */
#define	EV_FMAND	0x00000100	/* Text before () must be func name */
#define	EV_FCHECK	0x00000200	/* Check text before () for function */
#define	EV_STRIP	0x00000400	/* Strip one level of brackets */
#define	EV_EVAL		0x00000800	/* Evaluate results before returning */
#define	EV_STRIP_TS	0x00001000	/* Strip trailing spaces */
#define	EV_STRIP_LS	0x00002000	/* Strip leading spaces */
#define	EV_STRIP_ESC	0x00004000	/* Strip one level of \ characters */
#define	EV_STRIP_AROUND	0x00008000	/* Strip {} only at ends of string */
#define	EV_TOP		0x00010000	/* This is a toplevel call to eval() */
#define	EV_NOTRACE	0x00020000	/* Don't trace this call to eval */
#define EV_NO_COMPRESS  0x00040000      /* Don't compress spaces. */
#define EV_NO_LOCATION	0x00080000	/* Supresses %l */
#define EV_NOFCHECK	0x00100000	/* Do not evaluate functions! */

/* Message forwarding directives */

#define	MSG_PUP_ALWAYS	1	/* Always forward msg to puppet own */
#define	MSG_INV		2	/* Forward msg to contents */
#define	MSG_INV_L	4	/* ... only if msg passes my @listen */
#define	MSG_INV_EXITS	8	/* Forward through my audible exits */
#define	MSG_NBR		16	/* Forward msg to neighbors */
#define	MSG_NBR_A	32	/* ... only if I am audible */
#define	MSG_NBR_EXITS	64	/* Also forward to neighbor exits */
#define	MSG_NBR_EXITS_A	128	/* ... only if I am audible */
#define	MSG_LOC		256	/* Send to my location */
#define	MSG_LOC_A	512	/* ... only if I am audible */
#define	MSG_FWDLIST	1024	/* Forward to my fwdlist members if aud */
#define	MSG_ME		2048	/* Send to me */
#define	MSG_S_INSIDE	4096	/* Originator is inside target */
#define	MSG_S_OUTSIDE	8192	/* Originator is outside target */
#define MSG_HTML	16384   /* Don't send \r\n */
#define	MSG_ME_ALL	(MSG_ME|MSG_INV_EXITS|MSG_FWDLIST)
#define	MSG_F_CONTENTS	(MSG_INV)
#define	MSG_F_UP	(MSG_NBR_A|MSG_LOC_A)
#define	MSG_F_DOWN	(MSG_INV_L)

/* Look primitive directives */

#define	LK_IDESC	0x0001
#define	LK_OBEYTERSE	0x0002
#define	LK_SHOWATTR	0x0004
#define	LK_SHOWEXIT	0x0008
#define LK_SHOWVRML	0x0010

/* Quota types */
#define QTYPE_ALL 0
#define QTYPE_ROOM 1
#define QTYPE_EXIT 2
#define QTYPE_THING 3
#define QTYPE_PLAYER 4

/* Exit visibility precalculation codes */

#define	VE_LOC_XAM	0x01	/* Location is examinable */
#define	VE_LOC_DARK	0x02	/* Location is dark */
#define	VE_BASE_DARK	0x04	/* Base location (pre-parent) is dark */

/* Signal handling directives */

#define	SA_EXIT		1	/* Exit, and dump core */
#define	SA_DFLT		2	/* Try to restart on a fatal error */

#define	STARTLOG(key,p,s) \
	if ((((key) & mudconf.log_options) != 0) && start_log(p, s, key)) {
#define	ENDLOG \
	end_log(); }
#define	LOG_SIMPLE(key,p,s,m) \
	STARTLOG(key,p,s) \
		log_text(m); \
	ENDLOG

#define	test_top()		((mudstate.qfirst != NULL) ? 1 : 0)
#define	controls(p,x)		Controls(p,x)

#define safe_crlf(b,p)		safe_copy_known_str("\r\n",2,(b),(p),(LBUF_SIZE-1))
#define safe_ansi_normal(b,p)	safe_copy_known_str(ANSI_NORMAL,4,(b),(p),(LBUF_SIZE-1))
#define safe_nothing(b,p)	safe_copy_known_str("#-1",3,(b),(p),(LBUF_SIZE-1))
#define safe_noperm(b,p)	safe_copy_known_str("#-1 PERMISSION DENIED",21,(b),(p),(LBUF_SIZE-1))
#define safe_nomatch(b,p)	safe_copy_known_str("#-1 NO MATCH",12,(b),(p),(LBUF_SIZE-1))

#define safe_dbref(b,p,n) \
safe_chr('#',(b),(p)); \
safe_ltos((b),(p),(n));

#endif	/* __EXTERNS_H */
