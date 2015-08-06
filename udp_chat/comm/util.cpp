#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <string.h> 
#include <stdio.h>
#include "util.h"

#define MAXLINE 100

static int daemon_proc = 0;

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
		int		errno_save, n;
		char	buf[MAXLINE + 1];

		errnoflag = 1;

		errno_save = errno;		/* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
		vsnprintf(buf, MAXLINE, fmt, ap);	/* safe */
#else
		vsprintf(buf, fmt, ap);					/* not safe */
#endif
		n = strlen(buf);
		if (errnoflag)
			snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
			strcat(buf, "\n");

		if (daemon_proc) {
			syslog(level, buf);
		} else {
			fflush(stdout);		/* in case stdout and stderr are the same */
			fputs(buf, stderr);
			fflush(stderr);
		}
		return;
}

void log_error(const char*fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	return;
}

void log_message(const char*fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	return;
}

