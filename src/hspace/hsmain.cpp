// This file will serve as our connection between PennMUSH and
// HSpace.
#include "hscopyright.h"
#include "hspace.h"

extern "C"
{
  void hsInit(int);
  void hsDumpDatabases(void);
  void hsCommand(char *, char *, char *, char *, dbref);
  void hsCycle(void);
  char *hsFunction(char *, dbref, char **);
}

void
hsInit(int reboot)
{
	HSpace.InitSpace(reboot);
}

void
hsDumpDatabases(void)
{
	HSpace.DumpDatabases();
}

void
hsCommand(char *strCmd, char *switches, 
		  char *arg_left, char *arg_right, dbref player)
{
	HSpace.DoCommand(strCmd, switches, 
		arg_left, arg_right, player);
}

char *
hsFunction(char *strFunc, dbref executor, char **args)
{
	return HSpace.DoFunction(strFunc, executor, args);
}

void
hsCycle(void)
{
	HSpace.DoCycle();
}
