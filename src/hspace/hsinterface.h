#ifndef __HSINTERFACE_INCLUDED__
#define __HSINTERFACE_INCLUDED__

// #define PENNMUSH
#define TM3

extern "C" {

#include "autoconf.h"
#include "flags.h"
#include "attrs.h"
#include "db.h"
#include "externs.h"
#include "match.h"  
#include "command.h"

};

#ifdef TM3
extern char *wenv[10];
extern int numwenv;

#define getrandom(val)	((int) (makerandom() * (double)val))
#define moveto(a, b)	move_object(a, b);
#define look_room(a,b,c)	look_in(a,b,c)
#define set_name(a,b)	s_Name(a,b)
#define flag_broadcast(a,b,c,d,e)	raw_broadcast(b,c,d,e)

extern "C" double makerandom();
extern "C" void link_exit(dbref player, dbref exit, dbref dest);
#endif

enum HS_LOCKTYPE
{
	LOCK_NORMAL	= 0,
	LOCK_USE,
	LOCK_ZONE
};

// This is all taken directly from parse.h in PennMUSH
#ifdef PENNMUSH
typedef struct pe_info PE_Info;
#define PE_NOTHING		0
#define PE_COMPRESS_SPACES	0x00000001
#define PE_STRIP_BRACES		0x00000002
#define PE_EVALUATE		0x00000010
#define PE_FUNCTION_CHECK	0x00000020
#define PE_FUNCTION_MANDATORY	0x00000040
#define PE_LITERAL		0x00000100
#define PE_DEFAULT (PE_COMPRESS_SPACES | PE_STRIP_BRACES | \
		    PE_EVALUATE | PE_FUNCTION_CHECK)

#define PT_NOTHING	0
#define PT_BRACE	0x00000001
#define PT_BRACKET	0x00000002
#define PT_PAREN	0x00000004
#define PT_COMMA	0x00000008
#define PT_SEMI		0x00000010
#define PT_EQUALS	0x00000020
#define PT_SPACE	0x00000040
#define PT_DEFAULT PT_NOTHING
#endif

#ifdef TM3
#define PE_DEFAULT (EV_STRIP | EV_EVAL | EV_FCHECK)
#endif

// The CHSInterface class is used to allow HSpace to easily
// port to other types of game drivers.  Any game driver specific
// access methods should go in here whenever possible.
class CHSInterface
{
public:
	void CHZone(dbref, dbref);
	void AtrAdd(int, const char *, char *, int, int flags = NOTHING);
	void AtrDel(int, const char *, int);
	void SetFlag(int, int);
	void UnsetFlag(int, int);
	void SetLock(int, int, HS_LOCKTYPE); // Sets an object lock
	void NotifyContents(int, char *);
	void GetContents(int, int *, int);

	int MaxObjects(void);

	dbref CloneThing(dbref);
	dbref NoisyMatchThing(dbref, char *);
	dbref NoisyMatchRoom(dbref, char *);
	dbref NoisyMatchExit(dbref, char *);
	dbref GetLock(int, HS_LOCKTYPE);
	dbref ConsoleUser(int);
	dbref GetHome(dbref);

	BOOL PassesLock(int, int, HS_LOCKTYPE);
	BOOL HasFlag(int, int, int);
	BOOL ValidObject(dbref);
	BOOL AtrGet(int, const char *);
	BOOL LinkExits(dbref, dbref);
	BOOL UnlinkExits(dbref, dbref);

	char *EvalExpression(char *, int, int, int);
#ifdef PENNMUSH
	char m_buffer[4096];
#endif
#ifdef TM3
	char m_buffer[LBUF_SIZE];
#endif
	char *GetName(dbref);
};

extern CHSInterface hsInterface;

#endif // __HSINTERFACE_INCLUDED__
