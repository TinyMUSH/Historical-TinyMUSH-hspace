/* cque.c -- commands and functions for manipulating the command queue */
/* $Id$ */

#include "copyright.h"
#include "autoconf.h"

#include <signal.h>

#include "mudconf.h"
#include "config.h"
#include "db.h"
#include "htab.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "attrs.h"
#include "flags.h"
#include "powers.h"
#include "command.h"
#include "alloc.h"

extern int FDECL(a_Queue, (dbref, int));
extern void FDECL(s_Queue, (dbref, int));
extern int FDECL(QueueMax, (dbref));
extern dbref FDECL(match_thing, (dbref, char *));

/* ---------------------------------------------------------------------------
 * add_to: Adjust an object's queue or semaphore count.
 */

static int add_to(doer, player, am, attrnum)
dbref doer, player;
int am, attrnum;
{
	int num, aflags, alen;
	dbref aowner;
	char buff[20];
	char *atr_gotten;

	num = atoi(atr_gotten = atr_get(player, attrnum, &aowner,
					&aflags, &alen));
	free_lbuf(atr_gotten);
	num += am;
	if (num)
		ltos(buff, num);
	else
		*buff = '\0';
	atr_add(player, attrnum, buff, Owner(doer), aflags);
	return (num);
}

/* ---------------------------------------------------------------------------
 * give_que: Thread a queue block onto the high or low priority queue
 */

static void give_que(tmp)
BQUE *tmp;
{
	tmp->next = NULL;
	tmp->waittime = 0;

	/* Thread the command into the correct queue */

	if (Typeof(tmp->cause) == TYPE_PLAYER) {
		if (mudstate.qlast != NULL) {
			mudstate.qlast->next = tmp;
			mudstate.qlast = tmp;
		} else
			mudstate.qlast = mudstate.qfirst = tmp;
	} else {
		if (mudstate.qllast) {
			mudstate.qllast->next = tmp;
			mudstate.qllast = tmp;
		} else
			mudstate.qllast = mudstate.qlfirst = tmp;
	}
}

/* ---------------------------------------------------------------------------
 * que_want: Do we want this queue entry?
 */

static int que_want(entry, ptarg, otarg)
BQUE *entry;
dbref ptarg, otarg;
{
	if ((ptarg != NOTHING) && (ptarg != Owner(entry->player)))
		return 0;
	if ((otarg != NOTHING) && (otarg != entry->player))
		return 0;
	return 1;
}

/* ---------------------------------------------------------------------------
 * halt_que: Remove all queued commands from a certain player
 */

int halt_que(player, object)
dbref player, object;
{
	BQUE *trail, *point, *next;
	int numhalted, halt_all, i;
	int *dbrefs_array;

	numhalted = 0;
	halt_all = ((player == NOTHING) && (object == NOTHING)) ? 1 : 0; 
	if (halt_all)
	    dbrefs_array = (int *) calloc(mudstate.db_top, sizeof(int));

	/* Player queue */

	for (point = mudstate.qfirst; point; point = point->next)
		if (que_want(point, player, object)) {
			numhalted++;
			if (halt_all)
			    dbrefs_array[Owner(point->player)] += 1;
			point->player = NOTHING;
		} 

	/* Object queue */

	for (point = mudstate.qlfirst; point; point = point->next)
		if (que_want(point, player, object)) {
			numhalted++;
			if (halt_all)
			    dbrefs_array[Owner(point->player)] += 1;
			point->player = NOTHING;
		} 

	/* Wait queue */

	for (point = mudstate.qwait, trail = NULL; point; point = next)
		if (que_want(point, player, object)) {
			numhalted++;
			if (halt_all)
			    dbrefs_array[Owner(point->player)] += 1;
			if (trail)
				trail->next = next = point->next;
			else
				mudstate.qwait = next = point->next;
			free(point->text);
			free_qentry(point);
		} else
			next = (trail = point)->next;

	/* Semaphore queue */

	for (point = mudstate.qsemfirst, trail = NULL; point; point = next)
		if (que_want(point, player, object)) {
			numhalted++;
			if (halt_all)
			    dbrefs_array[Owner(point->player)] += 1;
			if (trail)
				trail->next = next = point->next;
			else
				mudstate.qsemfirst = next = point->next;
			if (point == mudstate.qsemlast)
				mudstate.qsemlast = trail;
			add_to(player, point->sem, -1, point->attr);
			free(point->text);
			free_qentry(point);
		} else
			next = (trail = point)->next;

	if (halt_all) {
	    for (i = 0; i < mudstate.db_top; i++) {
		if (dbrefs_array[i]) {
		    giveto(i, (mudconf.waitcost * dbrefs_array[i]));
		    s_Queue(i, 0);
		}
	    }
	    free(dbrefs_array);
	    return numhalted;
	}

	if (player == NOTHING)
		player = Owner(object);
	giveto(player, (mudconf.waitcost * numhalted));
	if (object == NOTHING)
		s_Queue(player, 0);
	else
		a_Queue(player, -numhalted);
	return numhalted;
}

/* ---------------------------------------------------------------------------
 * do_halt: Command interface to halt_que.
 */

void do_halt(player, cause, key, target)
dbref player, cause;
int key;
char *target;
{
	dbref player_targ, obj_targ;
	int numhalted;

	if ((key & HALT_ALL) && !(Can_Halt(player))) {
		notify(player, NOPERM_MESSAGE);
		return;
	}
	/* Figure out what to halt */

	if (!target || !*target) {
		obj_targ = NOTHING;
		if (key & HALT_ALL) {
			player_targ = NOTHING;
		} else {
			player_targ = Owner(player);
			if (Typeof(player) != TYPE_PLAYER)
				obj_targ = player;
		}
	} else {
		if (Can_Halt(player))
			obj_targ = match_thing(player, target);
		else
			obj_targ = match_controlled(player, target);

		if (obj_targ == NOTHING)
			return;
		if (key & HALT_ALL) {
			notify(player, "Can't specify a target and /all");
			return;
		}
		if (Typeof(obj_targ) == TYPE_PLAYER) {
			player_targ = obj_targ;
			obj_targ = NOTHING;
		} else {
			player_targ = NOTHING;
		}
	}

	numhalted = halt_que(player_targ, obj_targ);
	if (Quiet(player))
		return;
	if (numhalted == 1)
		notify(Owner(player), "1 queue entries removed.");
	else
		notify(Owner(player),
		       tprintf("%d queue entries removed.", numhalted));
}

/* ---------------------------------------------------------------------------
 * nfy_que: Notify commands from the queue and perform or discard them.
 */

int nfy_que(player, sem, attr, key, count)
dbref player, sem;
int attr, key, count;
{
	BQUE *point, *trail, *next;
	int num, aflags, alen;
	dbref aowner;
	char *str;

	if (attr) {
		str = atr_get(sem, attr, &aowner, &aflags, &alen);
		num = atoi(str);
		free_lbuf(str);
	} else {
		num = 1;
	}
	
	if (num > 0) {
		num = 0;
		for (point = mudstate.qsemfirst, trail = NULL; point; point = next) {
			if ((point->sem == sem) && 
			    ((point->attr == attr) || !attr)) {
				num++;
				if (trail)
					trail->next = next = point->next;
				else
					mudstate.qsemfirst = next = point->next;
				if (point == mudstate.qsemlast)
					mudstate.qsemlast = trail;

				/* Either run or discard the command */

				if (key != NFY_DRAIN) {
					give_que(point);
				} else {
					giveto(point->player,
					       mudconf.waitcost);
					a_Queue(Owner(point->player), -1);
					free(point->text);
					free_qentry(point);
				}
			} else {
				next = (trail = point)->next;
			}

			/* If we've notified enough, exit */

			if ((key == NFY_NFY) && (num >= count))
				next = NULL;
		}
	} else {
		num = 0;
	}

	/* Update the sem waiters count */

	if (key == NFY_NFY)
		add_to(player, sem, -count, (attr ? attr : A_SEMAPHORE));
	else
		atr_clr(sem, (attr ? attr: A_SEMAPHORE));

	return num;
}

/* ---------------------------------------------------------------------------
 * do_notify: Command interface to nfy_que
 */

void do_notify(player, cause, key, what, count)
dbref player, cause;
int key;
char *what, *count;
{
	dbref thing, aowner;
	int loccount, attr, aflags;
	ATTR *ap;
	char *obj;
	
	obj = parse_to(&what, '/', 0);
	init_match(player, obj, NOTYPE);
	match_everything(0);

	if ((thing = noisy_match_result()) < 0) {
		notify(player, "No match.");
	} else if (!controls(player, thing) && !Link_ok(thing)) {
		notify(player, NOPERM_MESSAGE);
	} else {
		if (!what || !*what) {
			ap = NULL;
		} else {
			ap = atr_str(what);
		}
		
		if (!ap) {
			attr = A_SEMAPHORE;
		} else {
			/* Do they have permission to set this attribute? */
			atr_pget_info(thing, ap->number, &aowner, &aflags);
			if (Set_attr(player, thing, ap, aflags)) {
				attr = ap->number;
			} else {
				notify_quiet(player, NOPERM_MESSAGE);
				return;
			}
		}

		if (count && *count)
			loccount = atoi(count);
		else
			loccount = 1;
		if (loccount > 0) {
			nfy_que(player, thing, attr, key, loccount);
			if (!(Quiet(player) || Quiet(thing))) {
				if (key == NFY_DRAIN)
					notify_quiet(player, "Drained.");
				else
					notify_quiet(player, "Notified.");
			}
		}
	}
}

/* ---------------------------------------------------------------------------
 * setup_que: Set up a queue entry.
 */

static BQUE *setup_que(player, cause, command, args, nargs, sargs)
dbref player, cause;
char *command, *args[], *sargs[];
int nargs;
{
	int a, tlen;
	BQUE *tmp;
	char *tptr;

	/* Can we run commands at all? */

	if (Halted(player))
		return NULL;

	/* make sure player can afford to do it */

	a = mudconf.waitcost;
	if (a && mudconf.machinecost && (random() % mudconf.machinecost) == 0)
		a++;
	if (!payfor(player, a)) {
		notify(Owner(player), "Not enough money to queue command.");
		return NULL;
	}
	/* Wizards and their objs may queue up to db_top+1 cmds. Players are
	 * limited to QUEUE_QUOTA. -mnp 
	 */

	a = QueueMax(Owner(player));
	if (a_Queue(Owner(player), 1) > a) {
		notify(Owner(player),
		    "Run away objects: too many commands queued.  Halted.");
		halt_que(Owner(player), NOTHING);

		/* halt also means no command execution allowed */
		s_Halted(player);
		return NULL;
	}
	/* We passed all the tests */

	/* Calculate the length of the save string */

	tlen = 0;
	if (command)
		tlen = strlen(command) + 1;
	if (nargs > NUM_ENV_VARS)
		nargs = NUM_ENV_VARS;
	for (a = 0; a < nargs; a++) {
		if (args[a])
			tlen += (strlen(args[a]) + 1);
	}
	if (sargs) {
		for (a = 0; a < NUM_ENV_VARS; a++) {
			if (sargs[a])
				tlen += (strlen(sargs[a]) + 1);
		}
	}
	/* Create the queue entry and load the save string */

	tmp = alloc_qentry("setup_que.qblock");
	tmp->comm = NULL;
	for (a = 0; a < NUM_ENV_VARS; a++) {
		tmp->env[a] = NULL;
	}
	for (a = 0; a < MAX_GLOBAL_REGS; a++) {
		tmp->scr[a] = NULL;
	}

	if (!(tptr = tmp->text = (char *) XMALLOC(tlen, "setup_que"))) {
		free_qentry(tmp);
		return (BQUE *) NULL;
	}
	if (command) {
		strcpy(tptr, command);
		tmp->comm = tptr;
		tptr += (strlen(command) + 1);
	}
	for (a = 0; a < nargs; a++) {
		if (args[a]) {
			strcpy(tptr, args[a]);
			tmp->env[a] = tptr;
			tptr += (strlen(args[a]) + 1);
		}
	}
	if (sargs) {
		for (a = 0; a < MAX_GLOBAL_REGS; a++) {
			if (sargs[a]) {
				strcpy(tptr, sargs[a]);
				tmp->scr[a] = tptr;
				tptr += (strlen(sargs[a]) + 1);
			}
		}
	}
	/* Load the rest of the queue block */

	tmp->player = player;
	tmp->waittime = 0;
	tmp->next = NULL;
	tmp->sem = NOTHING;
	tmp->attr = 0;
	tmp->cause = cause;
	tmp->nargs = nargs;
	return tmp;
}

/* ---------------------------------------------------------------------------
 * wait_que: Add commands to the wait or semaphore queues.
 */

void wait_que(player, cause, wait, sem, attr, command, args, nargs, sargs)
dbref player, cause, sem;
int wait, nargs, attr;
char *command, *args[], *sargs[];
{
	BQUE *tmp, *point, *trail;

	if (mudconf.control_flags & CF_INTERP)
		tmp = setup_que(player, cause, command, args, nargs, sargs);
	else
		tmp = NULL;
	if (tmp == NULL) {
		return;
	}
	if (wait != 0)
		tmp->waittime = time(NULL) + wait;
	tmp->sem = sem;
	tmp->attr = attr;
	if (sem == NOTHING) {

		/* No semaphore, put on wait queue if wait value specified.
		 * Otherwise put on the normal queue. 
		 */

		if (wait <= 0) {
			give_que(tmp);
		} else {
			for (point = mudstate.qwait, trail = NULL;
			     point && point->waittime <= tmp->waittime;
			     point = point->next) {
				trail = point;
			}
			tmp->next = point;
			if (trail != NULL)
				trail->next = tmp;
			else
				mudstate.qwait = tmp;
		}
	} else {
		tmp->next = NULL;
		if (mudstate.qsemlast != NULL)
			mudstate.qsemlast->next = tmp;
		else
			mudstate.qsemfirst = tmp;
		mudstate.qsemlast = tmp;
	}
}

/* ---------------------------------------------------------------------------
 * do_wait: Command interface to wait_que
 */

void do_wait(player, cause, key, event, cmd, cargs, ncargs)
dbref player, cause;
int key, ncargs;
char *event, *cmd, *cargs[];
{
	dbref thing, aowner;
	int howlong, num, attr, aflags;
	char *what;
	ATTR *ap;
	

	/* If arg1 is all numeric, do simple (non-sem) timed wait. */

	if (is_number(event)) {
		howlong = atoi(event);
		wait_que(player, cause, howlong, NOTHING, 0, cmd,
			 cargs, ncargs, mudstate.global_regs);
		return;
	}
	/* Semaphore wait with optional timeout */

	what = parse_to(&event, '/', 0);
	init_match(player, what, NOTYPE);
	match_everything(0);

	thing = noisy_match_result();
	if (!Good_obj(thing)) {
		notify(player, "No match.");
	} else if (!controls(player, thing) && !Link_ok(thing)) {
		notify(player, NOPERM_MESSAGE);
	} else {

		/* Get timeout, default 0 */

		if (event && *event && is_number(event)) {
			attr = A_SEMAPHORE;
			howlong = atoi(event);
		} else {
			attr = A_SEMAPHORE;
			howlong = 0;
		}

		if (event && *event && !is_number(event)) {
			ap = atr_str(event);
			if (!ap) {
				attr = mkattr(event);
				if (attr <= 0) {
					notify_quiet(player, "Invalid attribute.");
					return;
				}
				ap = atr_num(attr);
			}
			atr_pget_info(thing, ap->number, &aowner, &aflags);
			if (attr && Set_attr(player, thing, ap, aflags)) {
				attr = ap->number;
				howlong = 0;
			} else {
				notify_quiet(player, NOPERM_MESSAGE);
				return;
			}
		}
		
		num = add_to(player, thing, 1, attr);
		if (num <= 0) {

			/* thing over-notified, run the command immediately */
			thing = NOTHING;
			howlong = 0;
		}
		wait_que(player, cause, howlong, thing, attr, cmd,
			 cargs, ncargs, mudstate.global_regs);
	}
}

/* ---------------------------------------------------------------------------
 * que_next: Return the time in seconds until the next command should be
 * run from the queue.
 */

int NDECL(que_next)
{
	int min, this;
	BQUE *point;

	/* If there are commands in the player queue, we want to run them
	 * immediately. 
	 */

	if (test_top())
		return 0;

	/* If there are commands in the object queue, we want to run them
	 * after a one-second pause. 
	 */

	if (mudstate.qlfirst != NULL)
		return 1;

	/* Walk the wait and semaphore queues, looking for the smallest
	 * wait value.  Return the smallest value - 1, because
	 * the command gets moved to the player queue when it has 
	 * 1 second to go.  
	 */

	min = 1000;
	for (point = mudstate.qwait; point; point = point->next) {
		this = point->waittime - mudstate.now;
		if (this <= 2)
			return 1;
		if (this < min)
			min = this;
	}

	for (point = mudstate.qsemfirst; point; point = point->next) {
		if (point->waittime == 0)	/* Skip if no timeout */
			continue;
		this = point->waittime - mudstate.now;
		if (this <= 2)
			return 1;
		if (this < min)
			min = this;
	}
	return min - 1;
}

/* ---------------------------------------------------------------------------
 * do_second: Check the wait and semaphore queues for commands to remove.
 */

void NDECL(do_second)
{
	BQUE *trail, *point, *next;
	char *cmdsave;

	/* move contents of low priority queue onto end of normal one this
	 * helps to keep objects from getting out of control since
	 * its affects on other objects happen only after one
	 * second  this should allow @halt to be type before
	 * getting blown away  by scrolling text 
	 */

	if ((mudconf.control_flags & CF_DEQUEUE) == 0)
		return;

	cmdsave = mudstate.debug_cmd;
	mudstate.debug_cmd = (char *)"< do_second >";

	if (mudstate.qlfirst) {
		if (mudstate.qlast)
			mudstate.qlast->next = mudstate.qlfirst;
		else
			mudstate.qfirst = mudstate.qlfirst;
		mudstate.qlast = mudstate.qllast;
		mudstate.qllast = mudstate.qlfirst = NULL;
	}
	/* Note: the point->waittime test would be 0 except the command is
	 * being put in the low priority queue to be done in one
	 * second anyways 
	 */

	/* Do the wait queue */

	for (point = mudstate.qwait; point && point->waittime <= mudstate.now;
	     point = point->next) {
		mudstate.qwait = point->next;
		give_que(point);
	}

	/* Check the semaphore queue for expired timed-waits */

	for (point = mudstate.qsemfirst, trail = NULL; point; point = next) {
		if (point->waittime == 0) {
			next = (trail = point)->next;
			continue;	/* Skip if not timed-wait */
		}
		if (point->waittime <= mudstate.now) {
			if (trail != NULL)
				trail->next = next = point->next;
			else
				mudstate.qsemfirst = next = point->next;
			if (point == mudstate.qsemlast)
				mudstate.qsemlast = trail;
			add_to(point->player, point->sem, -1,
			       (point->attr ? point->attr : A_SEMAPHORE));
			point->sem = NOTHING;
			give_que(point);
		} else
			next = (trail = point)->next;
	}	
	mudstate.debug_cmd = cmdsave;
	return;
}

/* ---------------------------------------------------------------------------
 * do_top: Execute the command at the top of the queue
 */

int do_top(ncmds)
int ncmds;
{
    BQUE *tmp;
    dbref player;
    int count, i, numpipes, len;
    char *command, *cp, *cmdsave, *logbuf, *log_cmdbuf;
    struct timeval begin_time, end_time, obj_time;
    int used_time;
#ifdef TRACK_USER_TIME
    struct rusage usage;
    struct timeval b_utime, e_utime;
#endif

    if ((mudconf.control_flags & CF_DEQUEUE) == 0)
	return 0;

    cmdsave = mudstate.debug_cmd;
    mudstate.debug_cmd = (char *)"< do_top >";

    for (count = 0; count < ncmds; count++) {
	if (!test_top()) {
	    mudstate.debug_cmd = cmdsave;
	    for (i = 0; i < MAX_GLOBAL_REGS; i++) {
		*mudstate.global_regs[i] = '\0';
		mudstate.glob_reg_len[i] = 0;
	    }
	    return count;
	}
	player = mudstate.qfirst->player;
	if ((player >= 0) && !Going(player)) {
	    giveto(player, mudconf.waitcost);
	    mudstate.curr_enactor = mudstate.qfirst->cause;
	    mudstate.curr_player = player;
	    a_Queue(Owner(player), -1);
	    mudstate.qfirst->player = NOTHING;
	    if (!Halted(player)) {

		/* Load scratch args */
		
		for (i = 0; i < MAX_GLOBAL_REGS; i++) {
		    if (mudstate.qfirst->scr[i]) {
			len = strlen(mudstate.qfirst->scr[i]);
			bcopy(mudstate.qfirst->scr[i],
			      mudstate.global_regs[i],
			      len + 1);
			mudstate.glob_reg_len[i] = len;
		    } else {
			*mudstate.global_regs[i] = '\0';
			mudstate.glob_reg_len[i] = 0;
		    }
		}

		command = mudstate.qfirst->comm;
		tmp = mudstate.qfirst;
		while (command && (mudstate.qfirst == tmp)) {
		    cp = parse_to(&command, ';', 0);
		    if (cp && *cp) {
			numpipes = 0;
			while (command && (*command == '|') &&
			       (mudstate.qfirst == tmp) &&
			       (numpipes < mudconf.ntfy_nest_lim)) {
			    command++;
			    numpipes++;
			    mudstate.inpipe = 1;
			    mudstate.poutnew = alloc_lbuf("process_command.pipe");
			    mudstate.poutbufc = mudstate.poutnew;
			    mudstate.poutobj = player;

			    /* No lag check on piped commands */
			    process_command(player,
					    mudstate.qfirst->cause,
					    0, cp,
					    mudstate.qfirst->env,
					    mudstate.qfirst->nargs);

			    if (mudstate.pout) {
				free_lbuf(mudstate.pout);
				mudstate.pout = NULL;
			    }

			    *mudstate.poutbufc = '\0';
			    mudstate.pout = mudstate.poutnew;
			    cp = parse_to(&command, ';', 0);
			} 
			mudstate.inpipe = 0;

			/* Is the queue still linked like we think it is? */
			if (mudstate.qfirst != tmp) {
			    if (mudstate.pout) {
				free_lbuf(mudstate.pout);
				mudstate.pout = NULL;
			    }
			    break;
			}

#ifndef NO_LAG_CHECK
			get_tod(&begin_time);
#ifdef TRACK_USER_TIME
			getrusage(RUSAGE_SELF, &usage);
			b_utime.tv_sec = usage.ru_utime.tv_sec;
			b_utime.tv_usec = usage.ru_utime.tv_usec;
#endif
#endif /* ! NO_LAG_CHECK */

			log_cmdbuf = process_command(player,
						     mudstate.qfirst->cause,
						     0, cp,
						     mudstate.qfirst->env,
						     mudstate.qfirst->nargs);

			if (mudstate.pout) {
			    free_lbuf(mudstate.pout);
			    mudstate.pout = NULL;
			}

#ifndef NO_LAG_CHECK
			get_tod(&end_time);
#ifdef TRACK_USER_TIME
			getrusage(RUSAGE_SELF, &usage);
			e_utime.tv_sec = usage.ru_utime.tv_sec;
			e_utime.tv_usec = usage.ru_utime.tv_usec;
#endif
			used_time = msec_diff(end_time, begin_time);
			if ((used_time / 1000) >= mudconf.max_cmdsecs) {
			    STARTLOG(LOG_PROBLEMS, "CMD", "CPU")
  			    log_name_and_loc(player);
			    logbuf = alloc_lbuf("do_top.LOG.cpu");
			    sprintf(logbuf, " queued command taking %.2f secs (enactor #%d): ", (double) (used_time / 1000), mudstate.qfirst->cause);
			    log_text(logbuf);
			    free_lbuf(logbuf);
			    log_text(log_cmdbuf);
			    ENDLOG
			}

#ifndef NO_TIMECHECKING
			/* Don't use msec_add(), this is more accurate */

			obj_time = Time_Used(player);
#ifndef TRACK_USER_TIME
			obj_time.tv_usec += end_time.tv_usec -
			    begin_time.tv_usec;
                        obj_time.tv_sec += end_time.tv_sec -
                            begin_time.tv_sec;
#else
			obj_time.tv_usec += e_utime.tv_usec -
			    b_utime.tv_usec;
                        obj_time.tv_sec += e_utime.tv_sec -
                            b_utime.tv_sec;
#endif /* ! TRACK_USER_TIME */
                        if (obj_time.tv_usec < 0) {
                            obj_time.tv_usec += 1000000;
                            obj_time.tv_sec--;
                        } else if (obj_time.tv_usec >= 1000000) {
                            obj_time.tv_sec += obj_time.tv_usec / 1000000;
                            obj_time.tv_usec = obj_time.tv_usec % 1000000;
                        }
                        s_Time_Used(player, obj_time);
#endif /* ! NO_TIMECHECKING */
#endif /* ! NO_LAG_CHECK */
		    }
		}
	    }
	}

	if (mudstate.qfirst) {
	    tmp = mudstate.qfirst;
	    mudstate.qfirst = mudstate.qfirst->next;
	    free(tmp->text);
	    free_qentry(tmp);
	}
	if (!mudstate.qfirst)	/* gotta check this, as the value's changed */
	    mudstate.qlast = NULL;
    }

    for (i = 0; i < MAX_GLOBAL_REGS; i++) {
	*mudstate.global_regs[i] = '\0';
	mudstate.glob_reg_len[i] = 0;
    }
    mudstate.debug_cmd = cmdsave;
    return count;
}

/* ---------------------------------------------------------------------------
 * do_ps: tell player what commands they have pending in the queue
 */

static void show_que(player, key, queue, qtot, qent, qdel,
		     player_targ, obj_targ, header)
dbref player, player_targ, obj_targ;
int key, *qtot, *qent, *qdel;
BQUE *queue;
const char *header;
{
	BQUE *tmp;
	char *bp, *bufp;
	int i;
	ATTR *ap;

	*qtot = 0;
	*qent = 0;
	*qdel = 0;
	for (tmp = queue; tmp; tmp = tmp->next) {
		(*qtot)++;
		if (que_want(tmp, player_targ, obj_targ)) {
			(*qent)++;
			if (key == PS_SUMM)
				continue;
			if (*qent == 1)
				notify(player,
				       tprintf("----- %s Queue -----",
					       header));
			bufp = unparse_object(player, tmp->player, 0);
			if ((tmp->waittime > 0) && (Good_obj(tmp->sem))) {
			    /* A minor shortcut. We can never timeout-wait
			     * on a non-Semaphore attribute.
			     */
			    notify(player,
				   tprintf("[#%d/%d] %s:%s",
					   tmp->sem,
					   tmp->waittime - mudstate.now,
					   bufp, tmp->comm));
			} else if (tmp->waittime > 0) {
				notify(player,
				       tprintf("[%d] %s:%s",
					       tmp->waittime - mudstate.now,
					       bufp, tmp->comm));
			} else if (Good_obj(tmp->sem)) {
			    if (tmp->attr == A_SEMAPHORE) {
				notify(player,
				       tprintf("[#%d] %s:%s", tmp->sem,
					       bufp, tmp->comm));
			    } else {
				ap = atr_num(tmp->attr);
				if (ap && ap->name) {
				    notify(player,
					   tprintf("[#%d/%s] %s:%s", tmp->sem,
						   ap->name,
						   bufp, tmp->comm));
				} else {
				    notify(player,
					   tprintf("[#%d] %s:%s", tmp->sem,
						   bufp, tmp->comm));
				}
			    }
			} else {
				notify(player,
				       tprintf("%s:%s", bufp, tmp->comm));
			}
			bp = bufp;
			if (key == PS_LONG) {
				for (i = 0; i < (tmp->nargs); i++) {
					if (tmp->env[i] != NULL) {
						safe_str((char *)"; Arg",
							 bufp, &bp);
						safe_chr(i + '0', bufp, &bp);
						safe_str((char *)"='",
							 bufp, &bp);
						safe_str(tmp->env[i],
							 bufp, &bp);
						safe_chr('\'', bufp, &bp);
					}
				}
				*bp = '\0';
				bp = unparse_object(player, tmp->cause, 0);
				notify(player,
				       tprintf("   Enactor: %s%s",
					       bp, bufp));
				free_lbuf(bp);
			}
			free_lbuf(bufp);
		} else if (tmp->player == NOTHING) {
			(*qdel)++;
		}
	}
	return;
}

void do_ps(player, cause, key, target)
dbref player, cause;
int key;
char *target;
{
	char *bufp;
	dbref player_targ, obj_targ;
	int pqent, pqtot, pqdel, oqent, oqtot, oqdel, wqent, wqtot, sqent,
	 sqtot, i;

	/* Figure out what to list the queue for */

	if ((key & PS_ALL) && !(See_Queue(player))) {
		notify(player, NOPERM_MESSAGE);
		return;
	}
	if (!target || !*target) {
		obj_targ = NOTHING;
		if (key & PS_ALL) {
			player_targ = NOTHING;
		} else {
			player_targ = Owner(player);
			if (Typeof(player) != TYPE_PLAYER)
				obj_targ = player;
		}
	} else {
		player_targ = Owner(player);
		obj_targ = match_controlled(player, target);
		if (obj_targ == NOTHING)
			return;
		if (key & PS_ALL) {
			notify(player, "Can't specify a target and /all");
			return;
		}
		if (Typeof(obj_targ) == TYPE_PLAYER) {
			player_targ = obj_targ;
			obj_targ = NOTHING;
		}
	}
	key = key & ~PS_ALL;

	switch (key) {
	case PS_BRIEF:
	case PS_SUMM:
	case PS_LONG:
		break;
	default:
		notify(player, "Illegal combination of switches.");
		return;
	}

	/* Go do it */

	show_que(player, key, mudstate.qfirst, &pqtot, &pqent, &pqdel,
		 player_targ, obj_targ, "Player");
	show_que(player, key, mudstate.qlfirst, &oqtot, &oqent, &oqdel,
		 player_targ, obj_targ, "Object");
	show_que(player, key, mudstate.qwait, &wqtot, &wqent, &i,
		 player_targ, obj_targ, "Wait");
	show_que(player, key, mudstate.qsemfirst, &sqtot, &sqent, &i,
		 player_targ, obj_targ, "Semaphore");

	/* Display stats */

	bufp = alloc_mbuf("do_ps");
	if (See_Queue(player))
		sprintf(bufp, "Totals: Player...%d/%d[%ddel]  Object...%d/%d[%ddel]  Wait...%d/%d  Semaphore...%d/%d",
			pqent, pqtot, pqdel, oqent, oqtot, oqdel,
			wqent, wqtot, sqent, sqtot);
	else
		sprintf(bufp, "Totals: Player...%d/%d  Object...%d/%d  Wait...%d/%d  Semaphore...%d/%d",
		    pqent, pqtot, oqent, oqtot, wqent, wqtot, sqent, sqtot);
	notify(player, bufp);
	free_mbuf(bufp);
}

/* ---------------------------------------------------------------------------
 * do_queue: Queue management
 */

void do_queue(player, cause, key, arg)
dbref player, cause;
int key;
char *arg;
{
	BQUE *point;
	int i, ncmds, was_disabled;

	was_disabled = 0;
	if (key == QUEUE_KICK) {
		i = atoi(arg);
		if ((mudconf.control_flags & CF_DEQUEUE) == 0) {
			was_disabled = 1;
			mudconf.control_flags |= CF_DEQUEUE;
			notify(player, "Warning: automatic dequeueing is disabled.");
		}
		ncmds = do_top(i);
		if (was_disabled)
			mudconf.control_flags &= ~CF_DEQUEUE;
		if (!Quiet(player))
			notify(player,
			       tprintf("%d commands processed.", ncmds));
	} else if (key == QUEUE_WARP) {
		i = atoi(arg);
		if ((mudconf.control_flags & CF_DEQUEUE) == 0) {
			was_disabled = 1;
			mudconf.control_flags |= CF_DEQUEUE;
			notify(player, "Warning: automatic dequeueing is disabled.");
		}
		/* Handle the wait queue */

		for (point = mudstate.qwait; point; point = point->next) {
			point->waittime = -i;
		}

		/* Handle the semaphore queue */

		for (point = mudstate.qsemfirst; point; point = point->next) {
			if (point->waittime > 0) {
				point->waittime -= i;
				if (point->waittime <= 0)
					point->waittime = -1;
			}
		}

		do_second();
		if (was_disabled)
			mudconf.control_flags &= ~CF_DEQUEUE;
		if (Quiet(player))
			return;
		if (i > 0)
			notify(player,
			    tprintf("WaitQ timer advanced %d seconds.", i));
		else if (i < 0)
			notify(player,
			    tprintf("WaitQ timer set back %d seconds.", i));
		else
			notify(player,
			       "Object queue appended to player queue.");

	}
}
