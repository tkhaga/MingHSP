/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>

#include "mhsperror.h"

/* minghsp.cでも同名の定義を使用(将来は統一) */
#define MHSP_STRMAX 0x1000

char funcname[MHSP_STRMAX];
char *warnbuf = NULL;
int warnlimit;
int warnsize = 0;

void mhsp_error(const char *msg, ...)
{
	char buf[MHSP_STRMAX];
	va_list args;

	va_start(args, msg);

	strcpy(buf, "命令 \"");
	if (funcname[0] != '\0') {
		lstrcat(buf, funcname);
	}
	strcat(buf, "\" 内でエラーが発生しました\n\nエラーメッセージ: ");
	vsprintf(&buf[lstrlen(buf)], msg, args);
	MessageBox(NULL, buf, "MingHSP", MB_OK | MB_ICONSTOP);

	va_end(args);
	ExitProcess(0);
}

void mhsp_warn(const char *msg, ...)
{
	int len;
	char buf[MHSP_STRMAX];
	va_list args;

	va_start(args, msg);
	vprintf(msg, args);

	vsprintf(buf, msg, args);
	len = strlen(funcname) + strlen(buf) + 4;
	warnsize += len;
	if (warnsize < warnlimit && warnbuf != NULL) {
		warnbuf += sprintf(warnbuf, "%s: %s\r\n", funcname, buf);
	}

	va_end(args);
}

void setFuncName(const char *name)
{
	strcpy(funcname, name);
}

void setWarnBuf(char *buf, int limit)
{
	warnbuf = buf;
	warnlimit = limit;
}
