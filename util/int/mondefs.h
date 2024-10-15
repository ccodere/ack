/*
	These are defines used for monitor calls equivalent to the
	UNIX System 7 defines in different include files.
*/


#include "global.h"	/* ptr */
#include <time.h>

/** File access modes used for open() */
#define V7_O_RDONLY	0 // Open for reading only.
#define V7_O_WRONLY 1 // Open for writing only.
#define V7_O_RDWR   2 // Open for reading and writing.

/* From stat.h */

#define V7_S_ISUID 04000 /* Set-user-ID on execution. */
#define V7_S_ISGID 02000 /* Set-group-ID on execution. */

#define V7_S_IRUSR		  00400         /* read permission, owner */
#define V7_S_IWUSR		  00200         /* write permission, owner */
#define V7_S_IXUSR		  00100         /* execute/search permission, owner */

#define V7_S_IRGRP		  00040         /* read permission, group */
#define V7_S_IWGRP		  00020         /* write permission, group */
#define V7_S_IXGRP		  00010         /* execute/search permission, group */

#define V7_S_IROTH		  00004         /* read permission, other */
#define V7_S_IWOTH		  00002         /* write permission, other */
#define V7_S_IXOTH		  00001         /* execute/search permission, other */

/* From stat.h */

#define		V7_S_IFDIR	0040000	/* directory */
#define		V7_S_IFCHR	0020000	/* character special */
#define		V7_S_IFBLK	0060000	/* block special */
#define		V7_S_IFREG	0100000	/* regular */
#define		V7_S_IFMPC	0030000	/* multiplexed char special */
#define		V7_S_IFMPB	0070000	/* multiplexed block special */

/* Emulated tms structure */
struct emu_tms
{
 clock_t  tms_utime;  /* User CPU time. */
 clock_t  tms_stime;  /* System CPU time. */
 clock_t  tms_cutime; /* User CPU time of terminated child processes. */
 clock_t  tms_cstime; /* System CPU time of terminated child processes. */
};
