 //
//	error.c
//
//
//	Created by Victoria Mengqi LIU on 4/7/16.
//
//

#include "ial.h"
#include <stdarg.h>		/* ANSI C header file */
#include <syslog.h>		/* for syslog() */

int daemon_proc;		/* set nonzero by daemon_init() */

static void err_do(int, int, const char *, va_list);


/* Nonfatal error related to system call
 * Print msg and return */
void
err_re(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_do(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to system call
 * Print msg and terminate */
void
err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_do(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error related to system call
 * Print msg, dump core, and terminate */
void
err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_do(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

/* Nonfatal error unrelated to system call
 * Print msg and return */
void
err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_do(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to system call
 * Print msg and terminate */
void
err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_do(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}


/* Print msg and return to caller
 * Caller specifies "errnoflag" and "level" */
static void
err_do(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno;

#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);
#else
	vsprintf(buf, fmt, ap);
#endif
	n = strlen(buf);

	if (errnoflag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	strcat(buf, "\n");

	if (daemon_proc)
	{
		syslog(level, buf);
	}
	else
	{
		fflush(stdout); /* in case stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}

	return;
}
