#ifndef __HSTYPES_INCLUDED__
#define __HSTYPES_INCLUDED__

extern "C"
{
#include "externs.h"
}

typedef int BOOL;
typedef unsigned int UINT;

#define TRUE	1
#define FALSE	0

#ifdef WIN32
#include <string.h>
#define strcasecmp stricmp
#endif

#endif // __HSTYPES_INCLUDED__
