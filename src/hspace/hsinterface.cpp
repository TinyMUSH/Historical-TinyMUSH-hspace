#include "hscopyright.h"
#include <string.h>

#include "hstypes.h"
#include "hsinterface.h"

extern "C" {

#ifdef PENNMUSH

#include "externs.h"
#include "lock.h"
#include "game.h"
#include "match.h"

extern void process_expression (char *, char **, char const **,
			   dbref, dbref, dbref,
			   int, int, PE_Info *);

#endif
}

#ifdef TM3
char *wenv[10];
int numwenv = 0;
#endif

CHSInterface hsInterface;  // One instance of this.

// Adds an attribute with a value to an object.
void CHSInterface::AtrAdd(int obj, const char *atrname, char *val, int owner,
		int flags)
{
#ifdef PENNMUSH
	atr_add(obj, atrname, val, owner, flags);
#endif
#ifdef TM3
	int atr;
	char name[SBUF_SIZE];

	strcpy(name, atrname);
	atr = mkattr(name);

	atr_add_raw(obj, atr, val);
#endif
}

void CHSInterface::AtrDel(int obj, const char *atrname, int owner)
{
#ifdef PENNMUSH
	atr_clr(obj, atrname, owner);
#endif
#ifdef TM3
	int atr;
	char name[SBUF_SIZE];

	strcpy(name, atrname);
	atr = mkattr(atrname);
	atr_clr(obj, atr);
#endif
}

#ifdef TM3
extern int hsClonedObject;
#endif

// Clones an object and returns the dbref of the new clone.
dbref CHSInterface::CloneThing(dbref model)
{
	dbref clone;
#ifdef PENNMUSH
	struct lock_list *ll;

    clone = new_object();
	memcpy(REFDB(clone), REFDB(model), sizeof(struct object));
	// SET(db[clone].name, Name(model));
	Name(clone) = NULL;
	set_name(clone, Name(model));
	s_Pennies(clone, Pennies(model));
	atr_cpy(clone, model);

	db[clone].locks = NULL;
	for (ll = Locks(model); ll; ll = ll->next)
	{
		add_lock(clone, ll->type, dup_bool(ll->key));
	}
	db[clone].zone = db[model].zone;
	db[clone].parent = db[model].parent;
#ifdef USE_WARNINGS
	db[clone].warnings = 0;       /*
									 * zap warnings
									 */
#endif
#ifdef CREATION_TIMES
	/*
	 * We give the clone the same modification time that its
	 * other clone has, but update the creation time
	 */
	db[clone].creation_time = time((time_t *) 0);
#endif

	db[clone].contents = db[clone].location = db[clone].next = NOTHING;
#endif
#ifdef TM3
	char name[SBUF_SIZE];
	sprintf(name, "#%d", model);
	do_clone(Owner(model), Owner(model), 0, name, 0);
	clone = hsClonedObject;
	s_Flags(clone, Flags(clone) & ~HALT);
#endif
	return clone;
}

// Gets an attribute from an object and stores the value
// in the buffer.  It returns FALSE if the attribute was
// not found.  Otherwise, TRUE.
BOOL CHSInterface::AtrGet(int obj, const char *atrname)
{
#ifdef PENNMUSH
	ATTR *a;

	a = atr_get(obj, atrname);
	if (!a)
		return FALSE;

	strcpy(m_buffer, uncompress(a->value));
	return TRUE;
#endif
#ifdef TM3
	int atr;
	char *value;
	char name[SBUF_SIZE];

	strcpy(name, atrname);
	atr = mkattr(name);
	value = atr_get_raw((dbref) obj, atr);
	
	if (!value || !*value)
		return FALSE;

	strcpy(m_buffer, value);
	return TRUE;
#endif
}

int CHSInterface::MaxObjects(void)
{
#ifdef PENNMUSH
	return db_top;
#endif
#ifdef TM3
	return mudstate.db_top;
#endif
}

dbref CHSInterface::NoisyMatchThing(dbref player, char *name)
{
	dbref console;

#ifdef PENNMUSH
	console = noisy_match_result(player, name, TYPE_THING, 
		MAT_NEAR_THINGS);
#endif
#ifdef TM3
	init_match(player, name, TYPE_THING);
	match_neighbor();
	match_absolute();
	match_me();
	match_here();
	console = noisy_match_result();
#endif

	if (console == AMBIGUOUS)
		console = NOTHING;

	return console;
}

dbref CHSInterface::NoisyMatchRoom(dbref player, char *name)
{
	dbref room;

#ifdef PENNMUSH
	room = noisy_match_result(player, name, TYPE_ROOM, 
		MAT_ABSOLUTE | MAT_HERE);
#endif
#ifdef TM3
	init_match(player, name, TYPE_ROOM);
	match_absolute();
	match_here();
	room = noisy_match_result();
#endif

	if (room == AMBIGUOUS)
		room = NOTHING;

	return room;
}

// Specifically matches an exit for a given game driver
dbref CHSInterface::NoisyMatchExit(dbref player, char *name)
{
	dbref exit_m;

#ifdef PENNMUSH
  exit_m =
    match_result(player, name, TYPE_EXIT, MAT_EXIT);
#endif
#ifdef TM3
	init_match(player, name, TYPE_EXIT);
	match_exit();
	match_absolute();
	exit_m = match_result();
#endif

  if (exit_m == AMBIGUOUS)
	  exit_m = NOTHING;

  return exit_m;
}

void CHSInterface::SetFlag(int objnum, int flag)
{
#ifdef PENNMUSH
	Toggles(objnum) |= flag;
#endif
#ifdef TM3
	/* Since all HSPACE flags are in the third flag word, makes this a
	 * little easier... */
	s_Flags3(objnum, Flags3(objnum) | flag);
#endif
}

void CHSInterface::UnsetFlag(int objnum, int flag)
{
#ifdef PENNMUSH
	Toggles(objnum) &= ~flag;
#endif
#ifdef TM3
	/* Since all HSPACE flags are in the third flag word, makes this a
	 * little easier... */
	s_Flags3(objnum, Flags3(objnum) & ~flag);
#endif
}

// Checks to see if the specified object exists, isn't garbage,
// etc.
BOOL CHSInterface::ValidObject(dbref objnum)
{
#ifdef PENNMUSH
	if (GoodObject(objnum) && !IsGarbage(objnum))
		return TRUE;
	else
		return FALSE;
#endif
#ifdef TM3
	if (Good_obj(objnum))
		return TRUE;
	else
		return FALSE;
#endif
}

// Returns TRUE or FALSE depending on whether the object has
// the specified flag or not.
BOOL CHSInterface::HasFlag(dbref objnum, int type, int flag)
{
#ifdef PENNMUSH
	if ((Toggles(objnum) & flag) && (Typeof(objnum) == type))
		return TRUE;
	return FALSE;
#endif
#ifdef TM3
	/* Since all HSPACE flags are in the third flag word, makes this a
	 * little easier... */
	if (!(Typeof(objnum) == type))
		return FALSE;
	if (Flags(objnum) & flag)
		return TRUE;
	if (Flags2(objnum) & flag)
		return TRUE;
	if (Flags3(objnum) & flag)
		return TRUE;
	return FALSE;
#endif
}

// Can be used to set a type of lock on an object
void CHSInterface::SetLock(int objnum, int lockto, HS_LOCKTYPE lock)
{
#ifdef PENNMUSH
	char    tmp[32];
	struct boolexp *key;

	sprintf(tmp, "#%d", lockto);
	key = parse_boolexp(lockto, tmp);

	switch(lock)
	{
		case LOCK_USE:
			add_lock(objnum, Use_Lock, key);
			break;
		case LOCK_ZONE:
			add_lock(objnum, Zone_Lock, key);
	}
#endif
#ifdef TM3
	char    tmp[SBUF_SIZE];
	BOOLEXP *key;

	sprintf(tmp, "#%d", lockto);
	key = parse_boolexp(objnum, tmp, 1);

	switch(lock)
	{
		case LOCK_USE:
			atr_add_raw(objnum, A_LUSE, 
				unparse_boolexp_quiet(objnum, key));
			break;
		case LOCK_ZONE:
			atr_add_raw(objnum, A_LCONTROL, 
				unparse_boolexp_quiet(objnum, key));
	}
	free_boolexp(key);
#endif
}

dbref CHSInterface::GetLock(int objnum, HS_LOCKTYPE lock)
{
#ifdef PENNMUSH
	if (getlock(objnum, Use_Lock) == TRUE_BOOLEXP)
		return NOTHING;
	else
		return getlock(objnum, Use_Lock)->thing;
#endif
#ifdef TM3
	char *value;
	BOOLEXP *key;
	int aowner, aflags, alen;
	dbref lockobj;
	
	value = atr_get((dbref) objnum,  A_LUSE, &aowner, &aflags, &alen);
	key = parse_boolexp((dbref) objnum, value, 1);
	free_lbuf(value);
	if (key == TRUE_BOOLEXP) {
		free_boolexp(key);
		return NOTHING;
	} else {
		lockobj = key->thing;
		free_boolexp(key);
		return lockobj;
	}
#endif
}

dbref CHSInterface::ConsoleUser(int objnum)
{
	dbref dbUser;

	dbUser = GetLock(objnum, LOCK_USE);

	if (!dbUser || !objnum)
		return NOTHING;

#ifdef PENNMUSH
	if (IsPlayer(dbUser))
#endif
#ifdef TM3
	if (isPlayer(dbUser))
#endif
	{
		// If the user is not in the same location as the object,
		// set the lock to the object and return NOTHING.
#ifdef PENNMUSH
		if (Location(dbUser) != Location(objnum) || !HasFlag(dbUser, TYPE_PLAYER, PLAYER_CONNECT) && 
			IsPlayer(dbUser))
#endif
#ifdef TM3
		if (Location(dbUser) != Location(objnum) || !HasFlag(dbUser, TYPE_PLAYER, CONNECTED) && 
			isPlayer(dbUser))
#endif
		{
			SetLock(objnum, objnum, LOCK_USE);
				

			// Delete attribute from player.
			hsInterface.AtrDel(dbUser, "MCONSOLE", GOD);
#ifdef PENNMUSH
			notify_except(db[Location(objnum)].contents, dbUser,
				tprintf("%s unmans the %s.", Name(dbUser), Name(objnum)));
#endif
#ifdef TM3
			notify_except(Location(objnum), dbUser, dbUser,
				tprintf("%s unmans the %s.", Name(dbUser), Name(objnum)));
#endif		
			return NOTHING;
		}
	}

	return dbUser;
}

// Sends a message to the contents of an object, which is usually
// a room.
void CHSInterface::NotifyContents(int objnum, char *strMsg)
{
#ifdef PENNMUSH
  notify_except(db[objnum].contents, NOTHING, strMsg);
#endif
#ifdef TM3
	notify_all((dbref) objnum, objnum, strMsg);
#endif 
}

// Handles getting the contents of an object, which is often
// specific to each game driver.
void CHSInterface::GetContents(int loc, int *iArray, int type)
{
	int thing;
	int idx;

#ifdef PENNMUSH
	idx = 0;
	for (thing = db[loc].contents; GoodObject(thing); thing = Next(thing))
	{
		if (type != NOTYPE)
		{
			if (Typeof(thing) == type)
				iArray[idx++] = thing;
		}
		else
			iArray[idx++] = thing;
	}
	iArray[idx] = -1;
#endif
#ifdef TM3
	idx = 0;
	for (thing = Contents(loc); Good_obj(thing); thing = Next(thing))
	{
		if (type < GOODTYPE)
		{
			if (Typeof(thing) == type)
				iArray[idx++] = thing;
		}
		else
			iArray[idx++] = thing;
	}
	iArray[idx] = -1;
#endif
}

// Call this function to see if a given object can
// pass a type of lock on a target object.
BOOL CHSInterface::PassesLock(int obj, int target, 
							  HS_LOCKTYPE locktype)
{
#ifdef PENNMUSH
	if(locktype == LOCK_ZONE)
		return eval_lock(obj, target, Zone_Lock);
	else
		return eval_lock(obj, target, Use_Lock);
#endif
#ifdef TM3
	if(locktype == LOCK_ZONE)
		return could_doit(obj, target, A_LCONTROL);
	else
		return could_doit(obj, target, A_LUSE);
#endif
}

char *CHSInterface::EvalExpression(char *input, dbref executor,
					dbref caller, dbref enactor) 
{
#ifdef PENNMUSH
	static char tbuf[BUFFER_LEN];
#endif
#ifdef TM3
	static char tbuf[LBUF_SIZE];
#endif

	*tbuf = '\0';

#ifdef PENNMUSH
	char *bp;
	const char *p;

      	bp = tbuf;
      	p = input;
      	process_expression(tbuf, &bp, &p, executor, caller, enactor,
                         PE_DEFAULT, PT_DEFAULT, NULL);
      	*bp = '\0';
#endif
#ifdef TM3
	char *bp, *str;
	bp = tbuf;
	str = input;
	
	exec(tbuf, &bp, 0, executor, enactor, PE_DEFAULT, &str, wenv, numwenv);
	
	*bp = '\0';
	numwenv = 0;

	return tbuf;
#endif
}

void CHSInterface::CHZone(dbref room, dbref zone)
{
#ifdef PENNMUSH
	char zonename[32];
	char roomname[32];

	sprintf(zonename, "#%d", zone);
	sprintf(roomname, "#%d", room);

	do_chzone(GOD, roomname, zonename);
#endif
#ifdef TM3
	char zonename[SBUF_SIZE];
	char roomname[SBUF_SIZE];

	sprintf(zonename, "#%d", zone);
	sprintf(roomname, "#%d", room);

	do_chzone(GOD, GOD, 0, roomname, zonename);
	s_Flags2(room, Flags2(room) | CONTROL_OK);
#endif
}

BOOL CHSInterface::LinkExits(dbref sexit, dbref dexit)
{
	if (!sexit || !dexit)
		return FALSE;
	
#ifdef PENNMUSH
	if (!IsExit(sexit) || !IsExit(dexit))
		return FALSE;

	Location(dexit) = Home(sexit);
	Location(sexit) = Home(dexit);
#endif
#ifdef TM3
	if (!isExit(sexit) || !isExit(dexit))
		return FALSE;

	s_Location(dexit, Home(sexit));
	s_Location(sexit, Home(dexit));
#endif
	return TRUE;
}

BOOL CHSInterface::UnlinkExits(dbref sexit, dbref dexit)
{
	if (!sexit || !dexit)
		return FALSE;
	
#ifdef PENNMUSH
	if (!IsExit(sexit) || !IsExit(dexit))
		return FALSE;

	Location(dexit) = NOTHING;
	Location(sexit) = NOTHING;
#endif
#ifdef TM3
	if (!isExit(sexit) || !isExit(dexit))
		return FALSE;

	s_Location(dexit, NOTHING);
	s_Location(sexit, NOTHING);
#endif
	return TRUE;
}

dbref CHSInterface::GetHome(dbref dbObject)
{
	if (dbObject)
		return Home(dbObject);
	
	return NOTHING;
}

char *CHSInterface::GetName(dbref dbObject)
{
	if (dbObject)
		return Name(dbObject);

	return NULL;
}
