/*
	The MON instruction
*/

/* $Id$ */

#include	"sysidf.h"
#include	"io.h"
#include	"log.h"
#include	"alloc.h"
#include	"shadow.h"
#include	"m_sigtrp.h"
#include	"monstruct.h"
#include    "mondefs.h"
#include	"whatever.h"
#include    "config.h"

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(_POSIX_VERSION)
#efine HAS_UNISTD_H
#define HAS_SYS_WAIT_H
#define HAS_SYS_STAT_H
#define HAS_SYS_TIMES_H
#define HAS_SYS_TIMEB_H
#define HAS_FCNTL_H
#define HAS_ALARM
#define HAS_ACCESS
#define HAS_FORK
#define HAS_FTIME
#define HAS_KILL
#define HAS_PAUSE
#define HAS_SYNC
#define HAS_WAIT
#define HAS_TIMES
#define PLATFORM_UNIX
#endif

#include	<sys/types.h>
#include	<sys/stat.h>
#ifdef HAS_SYS_WAIT_H
#include	<sys/wait.h>
#endif
#include	<fcntl.h>
#include	<errno.h>
#include	<signal.h>
#include	<string.h>
#include	<time.h>
#ifdef HAS_UNISTD_H
#include	<unistd.h>
#endif
#include	<utime.h>
#ifdef	HAS_SYS_TIMEB_H				/* from system.h */
#include	<sys/timeb.h>
#endif

#ifndef S_IRUSR
#   ifdef S_IREAD
#	define S_IRUSR S_IREAD
#	define S_IWUSR S_IWRITE
#	define S_IXUSR S_IEXEC
#   else
#	define S_IRUSR 0400
#	define S_IWUSR 0200
#	define S_IXUSR 0100
#   endif
#endif

#ifndef S_IRGRP
#   ifdef S_IRUSR
#       define S_IRGRP (S_IRUSR>>3)
#       define S_IWGRP (S_IWUSR>>3)
#       define S_IXGRP (S_IXUSR>>3)
#   else
#       define S_IRGRP 0040
#       define S_IWGRP 0020
#       define S_IXGRP 0010
#   endif
#endif

#ifndef S_IROTH
#   ifdef S_IRUSR
#       define S_IROTH (S_IRUSR>>6)
#       define S_IWOTH (S_IWUSR>>6)
#       define S_IXOTH (S_IXUSR>>6)
#   else
#       define S_IROTH 0040
#       define S_IWOTH 0020
#       define S_IXOTH 0010
#   endif
#endif


#ifndef S_ISUID
#   define S_ISUID 04000
#endif

#ifndef S_ISGID
#   define S_ISGID 02000
#endif


extern int running;			/* from main.c */
extern int fd_limit;		/* from io.c */

#define	good_fd(fd)	(fd < fd_limit ? 1 : (errno = 9 /* EBADF */, 0))


#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"trap.h"
#include	"warn.h"
#include	"mem.h"

#define	INPUT		0
#define	OUTPUT		1

#define	DUPMASK		0x40

#define	INT2SIZE	max(wsize, 2L)
#define	INT4SIZE	max(wsize, 4L)

#define	pop_int()	((int) swpop())
#define	pop_int2()	((int) spop(INT2SIZE))
#define	pop_int4()	(spop(INT4SIZE))
#define	pop_intp()	((int) spop(psize))
#define	pop_uns2()	((unsigned int) upop(INT2SIZE))
#define	pop_unsp()	((unsigned int) upop(psize))
#define	pop_ptr()	(dppop())

#define	push_int(a)	(wpush((long)(a)))
#define	push_i2(a)	(npush((long)(a), INT2SIZE))
#define	push_i4(a)	(npush((long)(a), INT4SIZE))
#define	push_unsp(a)	(npush((long)(a), psize))

#define	push_err()	{ push_int(errno); push_int(errno); }

/************************************************************************
 *				Monitor calls.				*
 *									*
 *	The instruction "MON" expects a wsized integer on top of	*
 *	top of the stack, which identifies the call. Often there	*
 *	are also parameters following this number. The parameters	*
 *	were stacked in reverse order (C convention).			*
 *	The possible parameter types are :				*
 *									*
 *		1) int : integer of wordsize				*
 *		2) int2: integer with size max(2, wordsize)		*
 *		3) int4: integer with size max(4, wordsize)		*
 *		4) intp: integer with size of a pointer			*
 *		5) uns2: unsigned integer with size max(2, wordsize)	*
 *		6) unsp: unsigned integer with size of a pointer	*
 *		7) ptr : pointer into data space			*
 *									*
 *	After the call has been executed, a return code is present	*
 *	on top of the stack. If this return code equals zero, the call	*
 *	succeeded and the results of the call can be found right	*
 *	after the return code. A non zero return code indicates a	*
 *	failure.  In this case no results are available and the return	*
 *	code has been pushed twice.					*
 *									*
 *	Monitor calls such as "ioctl", "stat", "ftime", etc. work	*
 *	with a private buffer to be filled by the call. The fields	*
 *	of the buffer are written to EM-memory separately, possibly	*
 *	with some of the fields aligned.  To this end a number of	*
 *	transport routines are assembled in monstruct.[ch].		*
 *									*
 *	The EM report specifies a list of UNIX Version 7 -like system	*
 *	calls, not full access to the system calls on the underlying	*
 *	machine.  Therefore an attempt has been made to use or emulate	*
 *	the Version 7 system calls on the various machines.  A number	*
 *	of 4.1 BSD specific parameters have also been implemented.	*
 *									*
 ************************************************************************/

PRIVATE size buf_cnt[5];		/* Current sizes of the buffers */
PRIVATE char *buf[5];			/* Pointers to the buffers */

PRIVATE void check_buf(int n, size sz);
PRIVATE int savestr(int n, ptr addr);
PRIVATE int vec();

typedef void (*monCallHandler)(void);

static void fatal_error(void)
{
	trap(EBADMON);
}

/** ISO C90: void exit(int exit_code); terminate a process */
static void sys_exit(void)
{
#ifdef	LOGGING
		ES_def =
			((st_sh(SP) == UNDEFINED)
				|| (st_sh(SP + wsize-1) == UNDEFINED)) ?
			UNDEFINED : DEFINED;
#else
		ES_def = DEFINED;
#endif	/* LOGGING */
		ES = pop_int();
		running = 0;		/* stop the machine */
		LOG(("@m9 Exit: ES = %ld", ES));
}

#ifdef HAS_FORK
/** Create a new process.
    Can be done through POSIX call:  pid_t fork(void); */
static void sys_fork(void)
{
	int ppid;
	pid_t pid;
		ppid = getpid();
		if ((pid = fork()) == 0) {
			/* Child */
			init_ofiles(0);	/* Reinitialize */
			push_int(ppid);	/* Pid of parent */
			push_int(1);	/* Flag = 1 for child */
			push_int(0);
			LOG(("@m9 Fork: in child, ppid = %d", ppid));
		}
		else if (pid > 0) {	/* Parent */
			incr_mess_id();	/* Incr. id for next child */
			push_int(pid);	/* Pid of child */
			push_int(0);	/* Flag = 0 for parent */
			push_int(0);
			LOG(("@m9 Fork: in parent, cpid = %d", pid));
		}
		else {
			/* fork call failed */
			push_err();
			LOG(("@m4 Fork: failed, pid = %d, errno = %d",
				pid, errno));
		}
}
#endif

/** Read from a file.
    Can be done through POSIX call: ssize_t read(int fildes, void *buf, size_t nbyte); read from a file  */
static void sys_read(void)
{
	ssize_t n;				/* number actually read/written */
	int fd, fdnew;		/* file descriptors */
	ptr dsp1, dsp2, dsp3;	/* Data Space Pointers */
	int nbytes;			/* number to be read/written */
	int nr;
	char *cp;
	ptr addr;

		fd = pop_int();
		dsp1 = pop_ptr();
		nbytes = pop_intp();

		if (!good_fd(fd))
			goto read_error;
		if (nbytes < 0) {
			errno = 22;	/* EINVAL */
			goto read_error;
		}

		check_buf(0, (size)nbytes);
		if ((n = read(fd, buf[0], nbytes)) == -1)
			goto read_error;

#ifdef	LOGGING
		if (check_log("@m6")) {
			register int i;

			for (i = 0; i < n; i++) {
				LOG(("@m6 Read: char = '%c'", *(buf[0] + i)));
			}
		}
#endif	/* LOGGING */

		if (in_gda(dsp1) && !in_gda(dsp1 + (n-1))) {
			efault(WRGDAH);
			goto read_error;
		}

		if (!is_in_mem(dsp1, n)) {
			efault(WRUMEM);
			goto read_error;
		}

		for (	nr = n, cp = buf[0], addr = dsp1;
			nr;
			nr--, addr++, cp++
		) {
			if (in_stack(addr)) {
				ch_st_prot(addr);
				stack_loc(addr) = *cp;
				st_int(addr);
			}
			else {
				ch_dt_prot(addr);
				data_loc(addr) = *cp;
				dt_int(addr);
			}
		}

		push_unsp(n);
		push_int(0);
		LOG(("@m9 Read: succeeded, n = %d", n));
		return;

	read_error:
		push_err();
		LOG(("@m4 Read: failed, n = %d, errno = %d", n, errno));
		return;
}

/** Write to a file.
   Can be done through POSIX call: ssize_t write(int fildes, const void *buf, size_t nbyte); */
static void sys_write(void)
{
	int n;				/* number actually read/written */
	int fd, fdnew;		/* file descriptors */
	ptr dsp1, dsp2, dsp3;	/* Data Space Pointers */
	int nbytes;			/* number to be read/written */
	ptr addr;
	int nr;
	char *cp;

		fd = pop_int();
		dsp1 = pop_ptr();
		nbytes = pop_intp();

		if (!good_fd(fd))
			goto write_error;
		if (nbytes < 0) {
			errno = 22;	/* EINVAL */
			goto write_error;
		}

		if (in_gda(dsp1) && !in_gda(dsp1 + (nbytes-1))) {
			efault(WWGDAH);
			goto write_error;
		}
		if (!is_in_mem(dsp1, nbytes)) {
			efault(WWUMEM);
			goto write_error;
		}

#ifdef	LOGGING
		for (addr = dsp1; addr < dsp1 + nbytes; addr++) {
			if (mem_sh(addr) == UNDEFINED) {
				warning(in_stack(addr) ? WWLUNDEF : WWGUNDEF);
			}
		}
#endif	/* LOGGING */

		check_buf(0, (size)nbytes);
		for (	nr = nbytes, addr = dsp1, cp = buf[0];
			nr;
			nr--, addr++, cp++
		) {
			*cp = mem_loc(addr);
		}

#ifdef	LOGGING
		if (check_log("@m6")) {
			register int i;

			for (i = 0; i < nbytes; i++) {
				LOG(("@m6 write: char = '%c'", *(buf[0] + i)));
			}
		}
#endif	/* LOGGING */

		if ((n = write(fd, buf[0], nbytes)) == -1)
			goto write_error;

		push_unsp(n);
		push_int(0);
		LOG(("@m9 Write: succeeded, n = %d", n));
		return;

	write_error:
		push_err();
		LOG(("@m4 Write: failed, n = %d, nbytes = %d, errno = %d",
			n, nbytes, errno));
		return;
}

/** Open a File.
    Can be done through POSIX : int open(const char *path, int oflag, ... ); */
static void sys_open(void)
{
	int flag, oflag;			/* various flag parameters */
	ptr dsp1, dsp2, dsp3;	/* Data Space Pointers */
	int fd, fdnew;		/* file descriptors */

		dsp1 = pop_ptr();
		// open mode flag
		flag = pop_int();

		switch (flag)
		{
			case V7_O_RDONLY:
			   oflag = O_RDONLY;
			   break;
			case V7_O_WRONLY:
			    oflag = O_WRONLY;
				break;
			case V7_O_RDWR:
			    oflag = O_RDWR;
				break;
			default:
				push_err();
				LOG(("@m4 Open: failed, file = %lu, flag = %d, fd = %d, errno = %d",
						dsp1, flag, fd, errno));
		}


		if (!savestr(0, dsp1) || (fd = open(buf[0], oflag)) == -1) {
			push_err();
			LOG(("@m4 Open: failed, file = %lu, flag = %d, fd = %d, errno = %d",
					dsp1, flag, fd, errno));
		}
		else {
			push_int(fd);
			push_int(0);
			LOG(("@m9 Open: succeeded, file = %lu, flag = %d, fd = %d",
					dsp1, flag, fd));
		}
}


/** Close a file, equal to POSIX int close(int fildes); close a file  */
static void sys_close(void)
{
	int fd, fdnew;		/* file descriptors */
		fd = pop_int();
		if (!good_fd(fd) || close(fd) == -1) {
			push_err();
			LOG(("@m4 Close: failed, fd = %d, errno = %d",
				fd, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Close: succeeded"));
		}
}

/** POSIX 1996: pid_t wait(int *stat_loc); wait for a child process to stop or terminate */
#ifdef HAS_WAIT
static void sys_wait(void)
{
	int status;
	pid_t pid;			/* pid parameter typed int2 */
		if ((pid = wait(&status)) == -1) {
			push_err();
			LOG(("@m4 Wait: failed, status = %d, errno = %d",
				status, errno));
		}
		else {
			push_i2(pid);
			push_i2(status);
			push_int(0);
			LOG(("@m9 Wait: succeeded, status = %d, pid = %d",
					status, pid));
}
#endif


static mode_t v7mode2mode(int flag)
{
	mode_t mode = 0;
		if ((flag & V7_S_ISUID)==V7_S_ISUID)
		{
		   mode |= S_ISUID;
		}
		if ((flag & V7_S_ISGID)==V7_S_ISGID)
		{
		   mode |= S_ISGID;
		}
		if ((flag & V7_S_IRUSR)==V7_S_IRUSR)
		{
		   mode |= S_IRUSR;
		}
		if ((flag & V7_S_IWUSR)==V7_S_IWUSR)
		{
		   mode |= S_IWUSR;
		}
		if ((flag & V7_S_IXUSR)==V7_S_IXUSR)
		{
		   mode |= S_IXUSR;
		}
		if ((flag & V7_S_IRGRP)==V7_S_IRGRP)
		{
		   mode |= S_IRGRP;
		}
		if ((flag & V7_S_IWGRP)==V7_S_IWGRP)
		{
		   mode |= S_IWGRP;
		}
		if ((flag & V7_S_IXGRP)==V7_S_IXGRP)
		{
		   mode |= S_IXGRP;
		}
		if ((flag & V7_S_IROTH)==V7_S_IROTH)
		{
		   mode |= S_IROTH;
		}
		if ((flag & V7_S_IWOTH)==V7_S_IWOTH)
		{
		   mode |= S_IWOTH;
		}
		if ((flag & V7_S_IXOTH)==V7_S_IXOTH)
		{
		   mode |= S_IXOTH;
		}
	return mode;
}



static int mode2v7mode(mode_t mode)
{
	int localmode = 0;
		if ((mode & S_ISUID)==S_ISUID)
		{
		   localmode |= V7_S_ISUID;
		}
		if ((mode & S_ISGID)==S_ISGID)
		{
		   localmode |= V7_S_ISGID;
		}
		if ((mode & S_IRUSR)==S_IRUSR)
		{
		   localmode |= V7_S_IRUSR;
		}
		if ((mode & S_IWUSR)==S_IWUSR)
		{
		   localmode |= V7_S_IWUSR;
		}
		if ((mode & S_IXUSR)==S_IXUSR)
		{
		   localmode |= V7_S_IXUSR;
		}
		if ((mode & S_IRGRP)==S_IRGRP)
		{
		   localmode |= V7_S_IRGRP;
		}
		if ((mode & S_IWGRP)==S_IWGRP)
		{
		   localmode |= V7_S_IWGRP;
		}
		if ((mode & S_IXGRP)==S_IXGRP)
		{
		   localmode |= V7_S_IXGRP;
		}
		if ((mode & S_IROTH)==S_IROTH)
		{
		   localmode |= V7_S_IROTH;
		}
		if ((mode & S_IWOTH)==S_IWOTH)
		{
		   localmode |= V7_S_IWOTH;
		}
		if ((mode & S_IXOTH)==S_IXOTH)
		{
		   localmode |= V7_S_IXOTH;
		}
	return localmode;
}


/** Create a new file or rewrite an existing one
    This can be done through open() hence this is not supported.
*/
static void sys_creat(void)
{
	int flag;
	ptr    dsp1;
	dsp1 = pop_ptr();
	flag = pop_int();
    einval(WNOIMP);
	push_err();
	LOG(("@m4 Creat: failed, dsp1 = %lu, flag = %d, errno = %d",
		dsp1, flag, errno));
}

/** Link one file to another file
    Not supported in emulator.
*/
static void uns_link(void)
{
	ptr dsp1, dsp2, dsp3;		/* Data Space Pointers */

		dsp1 = pop_ptr();
		dsp2 = pop_ptr();
        einval(WNOIMP);
		push_err();
		LOG(("@m4 Link: failed, dsp1 = %lu, dsp2 = %lu, errno = %d", dsp1, dsp2, errno));
}

/** Remove a file/link. This is done through the libc call
    remove() */
static void emu_unlink(void)
{
	ptr dsp1;		/* Data Space Pointers */
	dsp1 = pop_ptr();
	if (!savestr(0, dsp1) || remove(buf[0]) != 0) {
			push_err();
			LOG(("@m4 Unlink: failed, dsp1 = %lu, errno = %d",
					dsp1, errno));
	}
	else {
			push_int(0);
			LOG(("@m9 Unlink: succeeded, dsp1 = %lu", dsp1));
	}
}

/** Change working directory
    Can be done through POSIX: int chdir(const char *path);  */
static void sys_chdir(void)
{
	ptr dsp1;		/* Data Space Pointers */
		dsp1 = pop_ptr();
		if (!savestr(0, dsp1) || chdir(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Chdir: failed, dsp1 = %lu, errno = %d",
				dsp1, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chdir: succeeded, dsp1 = %lu", dsp1));
		}
}

/** Make a directory, a special file, or a regular file
    Can be be done through: Opengroup X/OPEN (XSI) Extension: int mknod(const char *path, mode_t mode, dev_t dev);

    In the current implementation only creation of directories is supported so that it can be called through
    the POSIX call mkdir().

*/
static void sys_mknod(void)
{
	ptr dsp;
	ptr dsp1;
	dev_t address;
	int flag;
	mode_t mode;
	dsp1 = pop_ptr();
	flag = pop_int();
	mode = v7mode2mode(flag);
	address = pop_int2();
	if ((flag & V7_S_IFDIR)==V7_S_IFDIR)
	{
	  mode = mode | S_IFDIR;
#ifdef PLATFORM_UNIX
	if (!savestr(0, dsp1) || mkdir(buf[0], mode) == -1) {
			push_err();
			LOG(("@m4 Mknod: failed, dsp1 = %lu, mode = %d, address = %d, errno = %d",
					dsp1, mode, address, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Mknod: succeeded, dsp1 = %lu", dsp1));
		}
#else
	if (!savestr(0, dsp1) || mkdir(buf[0]) == -1) {
			push_err();
			LOG(("@m4 Mknod: failed, dsp1 = %lu, mode = %d, address = %d, errno = %d",
					dsp1, mode, address, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Mknod: succeeded, dsp1 = %lu", dsp1));
		}
#endif
	} else
	{
		trap(EBADMON);
	}
}

/** Change mode of a file
    Can be done through POSIX: int chmod(const char *path, mode_t mode); */
static void sys_chmod(void)
{
	ptr dsp1;
	int flag;
	mode_t mode;
		dsp1 = pop_ptr();
		flag = pop_int2();
		mode = v7mode2mode(flag);
		if (!savestr(0, dsp1) || chmod(buf[0], mode) == -1) {
			push_err();
			LOG(("@m4 Chmod: failed, dsp1 = %lu, mode = %d, errno = %d",
				dsp1, mode, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Chmod: succeeded, dsp1 = %lu", dsp1));
		}
}

/** Change owner and group of a file
    Can be done through POSIX call: int chown(const char *path, uid_t owner, gid_t group); */
static void uns_chown(void)
{
	ptr dsp1;
	int owner;
	int group;

	dsp1 = pop_ptr();
	owner = pop_int2();
	group = pop_int2();
    einval(WNOIMP);
	push_err();
	LOG(("@m4 Chown: failed, dsp1 = %lu, owner = %d, group = %d, errno = %d",
		dsp1, owner, group, errno));
}

/** Get file information
    Can be done through POSIX call: int stat(const char *restrict path, struct stat *restrict buf);
*/
static void sys_stat(void)
{
	struct stat st_buf;		/* private stat buffer */
	ptr dsp1, dsp2;


//!!! stat/fstat: Convert mode to correct format
		dsp1 = pop_ptr();	/* points to file-name space */
		dsp2 = pop_ptr();	/* points to EM-stat-buffer space */
		if (	!savestr(0, dsp1)
		||	stat(buf[0], &st_buf) == -1
		||	!stat2mem(dsp2, &st_buf)
		) {
			push_err();
			LOG(("@m4 Stat: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
				dsp1, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Stat: succeeded, dsp1 = %lu, dsp2 = %lu",
				dsp1, dsp2));
		}
}

/** Move the file pointer to specified offset
    Can be done through POSIX call: off_t lseek(int fildes, off_t offset, int whence); */
static void sys_lseek(void)
{
	int flag;			/* parameter for lseek */
	int whence;
	int fd;
	off_t off;

		whence = SEEK_SET;
		fd = pop_int();
		off = pop_int4();
		flag = pop_int();
		switch (flag)
		{
			case 0:
			   whence = SEEK_SET;
			   break;
			case 1:
			    whence = SEEK_CUR;
				break;
			case 2:
			    whence = SEEK_END;
				break;
		}
		LOG(("@m4 Lseek: fd = %d, off = %ld, whence = %d",
				fd, off, whence));

		if (!good_fd(fd) || (off = lseek(fd, off, whence)) == -1) {
			push_err();
			LOG(("@m9 Lseek: failed, errno = %d", errno));
		}
		else {
			push_i4(off);
			push_int(0);
			LOG(("@m9 Lseek: succeeded, pushed %ld", off));
		}
}

/** Get the process ID
    Can be done through POSIX: pid_t getpid(void);
*/
static void sys_getpid(void)
{
	pid_t pid;
	pid = getpid();
	push_i2(pid);
	LOG(("@m9 Getpid: succeeded, pid = %d", pid));
}

/** No equivalent today - disabled by default */
static void uns_mount(void)
{
	ptr dsp1, dsp2;
	int flag;

	dsp1 = pop_ptr();
	dsp2 = pop_ptr();
	flag = pop_int();
    einval(WNOIMP);
	push_err();
	LOG(("@m4 Mount: failed, dsp1 = %lu, dsp2 = %lu, flag = %d, errno = %d",
	   dsp1, dsp2, flag, errno));
}



/** No equivalent today - disabled by default */
static void uns_umount(void)
{
	int flag;
	ptr dsp1;
	ptr dsp2;
	dsp1 = pop_ptr();
	dsp2 = pop_ptr();
	flag = pop_int();
    einval(WNOIMP);
	push_err();
	LOG(("@m4 Umount: failed, dsp1 = %lu, dsp2 = %lu, flag = %d, errno = %d",
      dsp1, dsp2, flag, errno));
}


/** Not implemented in the interpreter */
static void uns_setuid(void)
{
	int userid = pop_int2();
    einval(WNOIMP);
	push_err();
	LOG(("@m4 Setuid: failed, userid = %d, errno = %d",
		userid, errno));
}

/** Proprietary, based on getuid() and geteuid(): get a real user ID of calling process
    and effective user ID of calling process.
 */
static void uns_getuid(void)
{
    einval(WNOIMP);
	push_err();
	LOG(("@m9 Getuid: failed"));
}

/** Unsupported
*/
static void uns_stime(void)
{
	unsigned long tm;			/* for stime call */
	tm = pop_int4();
	einval(WNOIMP);
	push_err();
	LOG(("@m4 Stime: failed, tm = %ld, errno = %d",
				tm, errno));
}


/** ptrace: Not implemented in emulator, will give an error. */
static void uns_ptrace(void)
{
	int request;
	pid_t pid;
	ptr dsp3;
	int n;
	request = pop_int();
	pid = pop_int2();
	dsp3 = pop_ptr();
	n = pop_int();		/* Data */
	einval(WPTRACEIMP);
	push_err();
	LOG(("@m4 Ptrace: failed, request = %d, pid = %d, addr = %lu, data = %d, errno = %d",
		request, pid, dsp3, n, errno));
}

#ifdef HAS_ALARM
/** Schedule an alarm signal.
    Can be done through POSIX call: unsigned int alarm(unsigned int seconds); */
static void sys_alarm(void)
{
	unsigned int seconds;
	seconds = pop_uns2();
	LOG(("@m9 Alarm(part 1) seconds = %u", seconds));
	seconds = alarm(seconds);
	push_i2(seconds);
	LOG(("@m9 Alarm(part 2) seconds = %u", seconds));
}
#endif

/** Get file information
    Can be done through POSIX call: int fstat(int fildes, struct stat *buf);
*/
static void sys_fstat(void)
{
	int fd;
	ptr dsp2;
	struct stat st_buf;		/* private stat buffer */

		fd = pop_int();
		dsp2 = pop_ptr();
		if (	!good_fd(fd)
		||	fstat(fd, &st_buf) == -1
		||	!stat2mem(dsp2, &st_buf)
		) {
			push_err();
			LOG(("@m4 Fstat: failed, fd = %d, dsp2 = %lu, errno = %d",
				fd, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Fstat: succeeded, fd = %d, dsp2 = %lu",
				fd, dsp2));
		}
}

/** Suspend the thread until a signal is received (POSIX 1996)
    int pause(void);
*/
#ifdef HAS_PAUSE
static void sys_pause(void)
{
	pause();
/*	einval(WNOIMP);
	push_err();
	LOG(("@m4 Pause: failed, errno = %d", errno));*/
}
#endif


/** Set file access and modification times
    Can be done through POSIX call: int utime(const char *path, const struct utimbuf *times); */
static void sys_utime(void)
{
	ptr dsp1;
	ptr dsp2;
	time_t actime;
	time_t modtime;
	struct utimbuf tm;

	dsp1 = pop_ptr();
	dsp2 = pop_ptr();
	if (memfault(dsp2, 2*INT4SIZE)) {
			push_err();
			LOG(("@m4 Utime: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
					dsp1, dsp2, errno));
			return;
		}
	actime = mem_ldu(dsp2, INT4SIZE);
	modtime = mem_ldu(dsp2 + INT4SIZE, INT4SIZE);
	tm.actime = actime;
	tm.modtime = modtime;
		if (!savestr(0, dsp1) || utime(buf[0], (struct utimbuf*) &tm) == -1) {
			push_err();
			LOG(("@m4 Utime: failed, dsp1 = %lu, dsp2 = %lu, errno = %d",
					dsp1, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Utime: succeeded, dsp1 = %lu, dsp2 = %lu",
					dsp1, dsp2));
		}
}

/** Determine accessibility of a file.
    Can be done through POSIX call: int access(const char *path, int amode); */
static void sys_access(void)
{
	int mode;
	ptr dsp1;
		dsp1 = pop_ptr();
		mode = pop_int();
		if (!savestr(0, dsp1) || access(buf[0], mode) == -1) {
			push_err();
			LOG(("@m4 Access: failed, dsp1 = %lu, mode = %d, errno = %d",
					dsp1, mode, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Access: succeeded, dsp1 = %lu, mode = %d",
				dsp1, mode));
		}
}

#ifdef WANT_NICE
/** Opengroup X/OPEN (XSI) Extension: change the nice value of a process
    int nice(int incr);
*/
static void sys_nice(void)
{
	int incr;			/* for nice call */
	incr = pop_int();
	nice(incr);
	LOG(("@m9 Nice: succeeded, incr = %d", incr));
}
#endif

/** Opengroup X/OPEN (XSI) Extension: Return date and time
    int ftime(struct timeb *tp);

    Internally calls the ISO C90 time_t time(time_t *tloc) function.
*/
static void sys_ftime(void)
{
	ptr dsp2;
	struct timeb tb_buf;		/* private timeb buffer */

	dsp2 = pop_ptr();
	tb_buf.time = time((time_t*)0);
	tb_buf.millitm = 0;
	tb_buf.timezone = timezone / 60;
	tb_buf.dstflag = daylight;
	if (!timeb2mem(dsp2, &tb_buf)) {
		push_err();
		LOG(("@m4 Ftime: failed, dsp2 = %lu, errno = %d",
				dsp2, errno));
	}
	else {
	    push_int(0);
	    LOG(("@m9 Ftime: succeeded, dsp2 = %lu", dsp2));
	}
}


/** Opengroup X/OPEN (XSI) Extension: Flushes the data
    to the file systems.
    void sync(void);
*/
#ifdef HAS_SYNC
static void sys_sync(void)
{
	sync();
	LOG(("@m9 Sync: succeeded"));
}
#endif


/** Send a signal to a process or a group of processes
    On non-UNIX platforms, will call raise in its own process. */
static void emu_kill(void)
{
	pid_t pid;
	pid_t currentpid;
	int sig;
	pid = pop_int2();
	sig = pop_int();
	currentpid = getpid();
#ifdef HAS_KILL
	if (kill(pid, sig) == -1) {
	  push_err();
	  LOG(("@m4 Kill: failed, pid = %d, sig = %d, errno = %d",
					pid, sig, errno));
		}
	else {
			push_int(0);
			LOG(("@m9 Kill: succeeded, pid = %d, sig = %d",
				pid, sig));
		}
#else
	if (pid == currentpid)
	{
		raise(sig);
		push_int(0);
		LOG(("@m9 Kill: succeeded, pid = %d, sig = %d",
			pid, sig));
	} else
	{
  	  einval(WNOIMP);
	  push_err();
	  LOG(("@m4 Kill: failed, pid = %d, sig = %d, errno = %d",
					pid, sig, errno));
	}
#endif
}

/** Duplicate an open file descriptor
   int dup(int fildes);
   int dup2(int fildes, int fildes2); */
static void sys_dup(void)
{
	int fd;
	int fdnew;

		fd = pop_int();
		fdnew = pop_int();
		if (fd & DUPMASK)
		{
			int fd1 = fd & ~DUPMASK;/* stripped */

			LOG(("@m4 Dup2: fd1 = %d, fdnew = %d", fd1, fdnew));
			if (!good_fd(fd1)) {
				fdnew = -1;
				goto dup2_error;
			}
			fdnew = dup2(fd1, fdnew);
		dup2_error:;
		}
		else
		{
			LOG(("@m4 Dup: fd = %d, fdnew = %d", fd, fdnew));
			fdnew = (!good_fd(fd) ? -1 : dup(fd));
		}

		if (fdnew == -1)
		{
			push_err();
			LOG(("@m4 Dup/Dup2: failed, fdnew = %d, errno = %d",
				fdnew, errno));
		}
		else
		{
			push_int(fdnew);
			push_int(0);
			LOG(("@m9 Dup/Dup2: succeeded, fdnew = %d", fdnew));
		}
}

/** Create an interprocess channel. Not implemented. */
static void uns_pipe(void)
{
	int pfds[2];
	einval(WNOIMP);
	push_err();
	LOG(("@m4 Pipe: failed, errno = %d", errno));
}


/** This is emulated using the clock() libc function,
    has getting this value in an interpreter makes no sense,
    as there is no way to differentiate interpreted monitor
    calls and those of the real underlying system. */
static void emu_times(void)
{
	ptr dsp2;
#ifdef HAS_TIMES
	struct tms tm_buf;
#else
	struct emu_tms tm_buf;		/* private tms buffer */
#endif
	dsp2 = pop_ptr();

#ifdef HAS_TIMES
		times(&tm_buf);
		if (!tms2mem(dsp2, &tm_buf)) {
			push_err();
			LOG(("@m4 Times: failed, dsp2 = %lu, errno = %d",
				dsp2, errno));
		}
		else {
			LOG(("@m9 Times: succeeded, dsp2 = %lu", dsp2));
		}
#else
	clock_t totaltime = clock();
	tm_buf.tms_stime = 0;
	tm_buf.tms_utime = totaltime;
	tm_buf.tms_cstime = 0;
	tm_buf.tms_cutime = 0;
	if (!tms2mem(dsp2, &tm_buf))
	{
			push_err();
			LOG(("@m4 Times: failed, dsp2 = %lu, errno = %d",
				dsp2, errno));
	}
	else {
			LOG(("@m9 Times: succeeded, dsp2 = %lu", dsp2));
	}
#endif
}

static void uns_profil(void)
{
	ptr dsp1;
	long nbytes;
	long off;
	long n;

	dsp1 = pop_ptr();	/* Buffer */
	nbytes = pop_intp();	/* Buffer size */
	off = pop_intp();	/* Offset */
	n = pop_intp();		/* Scale */
	einval(WPROFILIMP);
	push_err();
	LOG(("@m4 Profil: failed, dsp1 = %lu, nbytes = %d, offset = %d, scale = %d, errno = %d",
		dsp1, nbytes, off, n, errno));
}

/** Set-group-ID
    Can be done through POSIX call: int setgid(gid_t gid);
*/
static void uns_setgid(void)
{
	int groupid = pop_int2();
	einval(WNOIMP);
	push_err();
	push_err();
	LOG(("@m4 Setgid: failed, groupid = %d, errno = %d", groupid, errno));
}

/** Get the real group ID and effective group ID.
   Can be done through POSIX call: gid_t getgid(void) and
   gid_t getegid(void)
*/
static void uns_getgid(void)
{
	einval(WNOIMP);
	push_err();
	LOG(("@m9 Getgid: failed"));
}

/** Map UNIX signals to EM interrupts. */
static void sys_sigtrp(void)
{
	int trap_no;			/* for sigtrp; trap number */
	int old_trap_no;		/* for sigtrp; old trap number */
	int sig_no;			/* for sigtrp; signal number */

	trap_no = pop_int();
	sig_no = pop_int();

		if ((old_trap_no = do_sigtrp(trap_no, sig_no)) == -1) {
			push_err();
			LOG(("@m4 Sigtrp: failed, trap_no = %d, sig_no = %d, errno = %d",
					trap_no, sig_no, errno));
		}
		else {
			push_int(old_trap_no);
			push_int(0);
			LOG(("@m9 Sigtrp: succeeded, trap_no = %d, sig_no = %d, old_trap_no = %d",
					trap_no, sig_no, old_trap_no));
		}
}

/** Turn accounting on or off -- Unsupported */
static void uns_acct(void)
{
	ptr dsp1;
	dsp1 = pop_ptr();
	einval(WNOIMP);
	push_err();
	LOG(("@m4 Acct: failed, dsp1 = %lu, errno = %d", dsp1, errno));
}

/** Control a streams device. This is proprietary */
static void sys_ioctl(void)
{
	int fd;
	int request;
	ptr dsp2;
	fd = pop_int();
	request = pop_int();
	dsp2 = pop_ptr();
		if (!good_fd(fd) || do_ioctl(fd, request, dsp2) != 0) {
			push_err();
			LOG(("@m4 Ioctl: failed, fd = %d, request = %d, dsp2 = %lu, errno = %d",
				fd, request, dsp2, errno));
		}
		else {
			push_int(0);
			LOG(("@m9 Ioctl: succeeded, fd = %d, request = %d, dsp2 = %lu",
				fd, request, dsp2));
		}
}

/** Multiplexed file handling. Unsupported. */
static void sys_mpxcall(void)
{
	int request;
	ptr dsp1;
	request = pop_int();	/* Command */
	dsp1 = pop_ptr();	/* Vec */
	einval(WMPXIMP);
	push_err();
	LOG(("@m4 Mpxcall: failed, request = %d, dsp1 = %lu, errno = %d",
		request, dsp1, errno));
}

/** Execute a file (POSIX) */
static void sys_exec(void)
{
	char **envvec;			/* environment vector (exec) */
	char **argvec;			/* argument vector (exec) */
	ptr dsp1;
	ptr dsp2;
	ptr dsp3;
	dsp1 = pop_ptr();
	dsp2 = pop_ptr();
	dsp3 = pop_ptr();
		if (	!savestr(0, dsp1)
		||	!vec(1, 2, dsp2, &argvec)
		||	!vec(3, 4, dsp3, &envvec)
		||	/* execute results, ignore return code */
			(execve(buf[0], argvec, envvec), 1)
		) {
			push_err();
			LOG(("@m4 Exece: failed, dsp1 = %lu, dsp2 = %lu, dsp2 = %lu, errno = %d",
				dsp1, dsp2, dsp3, errno));
		}
}

/** set and get the file mode creation mask (POSIX) */
static void sys_umask(void)
{
	int modes;
	mode_t mode;
	mode_t oldmode;
	int oldmask;
	modes = pop_int2();
	mode = v7mode2mode(modes);
	oldmode = umask(mode);
	oldmask = mode2v7mode(oldmode);
	push_int(oldmask);
	LOG(("@m9 Umask: succeeded, mode = %d, oldmask = %d",
	  mode, oldmask));
}

/** unsupported */
static void uns_chroot(void)
{
	ptr dsp1;
	dsp1 = pop_ptr();
	einval(WNOIMP);
	push_err();
	LOG(("@m4 Chroot: failed, dsp1 = %lu, errno = %d", dsp1, errno));
}


static monCallHandler sys_handlers[] =
{
/*  0: NULL */    &fatal_error,
/*  1: exit() */  &sys_exit,
#ifdef HAS_FORK
/*  2: fork() */  &sys_fork,
#else
/*  2: fork() */  &fatal_error,
#endif
/*  3: read() */  &sys_read,
/*  4: write() */ &sys_write,
/*  5: open()  */ &sys_open,
/*  6: close() */ &sys_close,
#ifdef HAS_WAIT
/*  7: wait()  */ &sys_wait,
#else
/*  7: wait()  */ &fatal_error,
#endif
/*  8: creat() */ &sys_creat,/* Will return an error, not implemented. */
/*  9: link()  */ &uns_link,
/* 10: unlink()*/ &emu_unlink,
/* 11: NULL    */ &fatal_error,
/* 12: chdir() */ &sys_chdir,
/* 13: NULL    */ &fatal_error,
/* 14: mknod() */ &sys_mknod,
/* 15 chmod()  */ &sys_chmod,
/* 16  chown() */ &uns_chown,
/* 17: NULL    */ &fatal_error,
/* 18  stat() */  &sys_stat,
/* 19  lseek() */ &sys_lseek,
/* 20  getpid() */&sys_getpid,
/* 21  mount() */ &uns_mount,   /* Will return an error, not implemented. */
/* 22  umount() */&uns_umount,  /* Will return an error, not implemented. */
/* 23  setuid() */&uns_setuid,  /* Will return an error, not implemented. */
/* 24  getuid() */&uns_getuid,  /* Will return an error, not implemented. */
/* 25  stime() */ &uns_stime,   /* Will return an error, not implemented. */
/* 26  ptrace() */&uns_ptrace,  /* Will return an error, not implemented. */
#ifdef HAS_ALARM
/* 27  alarm() */ &sys_alarm,
#else
/* 27  alarm() */ &fatal_error,
#endif
/* 28  fstat() */ &sys_fstat,
#ifdef HAS_PAUSE
/* 29  pause() */ &sys_pause,
#else
/* 29  pause() */ &fatal_error,
#endif
/* 30  utime() */ &sys_utime,
/* 31: NULL    */ &fatal_error,
/* 32: NULL    */ &fatal_error,
/* 33  access() */&sys_access,
#ifdef WANT_NICE
/* 34  nice() */  &sys_nice,
#else
/* 34  nice() */  &fatal_error,
#endif
/* 35  ftime() */ &sys_ftime,
#ifdef HAS_SYNC
/* 36  sync() */  &sys_sync,
#else
/* 36  sync() */  &fatal_error,
#endif
/* 37  kill() */  &emu_kill,
/* 38: NULL    */ &fatal_error, /* On Linux x86/arm: This is rename() */
/* 39: NULL    */ &fatal_error, /* On Linux x86/arm This is mkdir() */
/* 40: NULL    */ &fatal_error, /* On Linux x86/arm This is rmdir() */
/* 41  dup & dup2() */ &sys_dup,
/* 42  pipe() */  &uns_pipe,   /* Will return an error, not implemented. */
/* 43  times() */ &emu_times,
/* 44  profil() */&uns_profil, /* Will return an error, not implemented. */
/* 45: NULL    */ &fatal_error,
/* 46  setgid() */&uns_setgid,  /* Will return an error, not implemented. */
/* 47  getgid() */&uns_getgid, /* Will return an error, not implemented. */
/* 48  sigtrp() */&sys_sigtrp,
/* 49: NULL    */ &fatal_error,
/* 50: NULL    */ &fatal_error,
/* 51  acct() */  &uns_acct,  /* Will return an error, not implemented. */
/* 52: NULL    */ &fatal_error,
/* 53: NULL    */ &fatal_error,
/* 54  ioctl() */ &sys_ioctl,
/* 55: NULL    */ &fatal_error,
/* 56  mpxcall() */&sys_mpxcall, /* Will return an error, not implemented. */
/* 57: NULL    */ &fatal_error,
/* 58: NULL    */ &fatal_error,
/* 59  exec() */  &sys_exec,    /* On Linux x86: This is: oldolduname */
/* 60  umask() */ &sys_umask,
/* 61  chroot() */&uns_chroot,  /* Will return an error, not implemented. */
/* 62: NULL    */ &fatal_error,
/* 63: NULL    */ &fatal_error
///* 64: NULL    */ &sys_rmdir,   /* UNIX 8th edition */
///* 65: NULL    */ &sys_mkdir    /* UNIX 8th edition */
};


void moncall(void)
{
	int monnr = pop_int();
	if ((monnr < 0) || (monnr > 65))
	{
		trap(EBADMON);
	}

	sys_handlers[monnr]();
}

/* Buffer administration */

PRIVATE void check_buf(int n, size sz)
{
	if (buf_cnt[n] == 0) {
		buf_cnt[n] = max(128, sz);
		buf[n] = Malloc(buf_cnt[n], "moncall buffer");
	}
	else if (buf_cnt[n] < sz) {
		buf_cnt[n] = allocfrac(sz);
		buf[n] = Realloc(buf[n], buf_cnt[n], "moncall buffer");
	}
}

PRIVATE int savestr(int n, ptr addr)
{
	register size len;
	register char *cp, ch;

	/* determine the length, carefully */
	len = 0;
	do {
		if (memfault(addr + len, 1L)) {
			return 0;
		}
		ch = mem_loc(addr + len);
		len++;
	} while (ch);

	/* allocate enough buffer space */
	check_buf(n, len);

	/* copy the string */
	cp = buf[n];
	do {
		*cp++ = ch = mem_loc(addr);
		addr++;
	} while (ch);

	return 1;
}

PRIVATE int vec(n1, n2, addr, vecvec)
	int n1, n2;
	ptr addr;
	char ***vecvec;
{
	register char *cp1, *cp2;
	ptr p, ldp;
	register int n_ent = 0;		/* number of entries */
	register size str = 0;		/* total string length */

	/* determine number of elements n_ent */
	p = addr;
	do {
		if (memfault(addr, psize)) {
			return 0;
		}
		ldp = mem_lddp(p);
		if (!savestr(n2, ldp)) {
			return 0;
		}
		str += strlen(buf[n2]) + 1;
		n_ent++;
		p += psize;
	} while (ldp);
	n_ent++;

	*vecvec = (char **) Malloc((size)(n_ent * sizeof (char *)),
					"argvec or envvec in exec()");
	check_buf(n1, str);

	/* copy the elements */
	for (	cp1 = buf[n1], n_ent = 0, p = addr;
		(ldp = mem_lddp(p)) != 0;
		p += psize, n_ent++
	) {
		if (!savestr(n2, ldp)) {
			return 0;
		}
		(*vecvec)[n_ent] = cp1;
		cp2 = buf[n2];
		while ((*cp1++ = *cp2++) != '\0') {
			/* nothing */
		}
	}
	(*vecvec)[n_ent] = 0;
	return 1;
}

int memfault(ptr addr, size length)
{
	/* centralizes (almost) all memory access tests in MON */
	if (!is_in_mem(addr, length)) {
		efault(WMONFLT);
		return 1;
	}
	return 0;
}

void efault(int wrn /* warning number */)
{
	warning(wrn);
	errno = 14;			/* EFAULT */
}

void einval(int wrn /* warning number */)
{
	warning(wrn);
	errno = 22;			/* EINVAL */
}
