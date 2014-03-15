
#include <stdio.h>
#include <stdarg.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "error.h"

char funcname[32];
char *warnbuf = NULL;
int warnlimit;
int warnsize = 0;
char buf[4096];

void warn_default(char *msg, ...)
{
  int funclen;
  va_list args;

  va_start(args, msg);
#ifdef WIN32
  vsprintf(buf, msg, args);
  funclen = strlen(funcname);
  warnsize += funclen + strlen(buf) + 4;
  if (warnsize < warnlimit && warnbuf != NULL)
  {
    strcat(warnbuf, funcname);
    strcat(warnbuf, ": ");
    warnbuf += funclen + 2;
    warnbuf += vsprintf(warnbuf, msg, args);
    strcat(warnbuf, "\n");
    warnbuf += 2;
  }
#else
  vprintf(msg, args);
#endif
  va_end(args);
}

void error_default(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
#ifdef WIN32
  strcpy(buf, "命令 \"");
  if (funcname[0] != '\0')
    strcat(buf, funcname);
  strcat(buf, "\" 内でエラーが発生しました\n\nエラーメッセージ: ");
  vsprintf(&buf[strlen(buf)], msg, args);
  MessageBox(NULL, buf, "MingHSP", MB_OK | MB_ICONSTOP);
#else
  vprintf(msg, args);
#endif
  va_end(args);
  exit(0);
}

void (*SWF_warn)(char *msg, ...) = warn_default;
void (*SWF_error)(char *msg, ...) = error_default;

void setSWFWarnFunction(void (*warnfunc)(char *msg, ...))
{
  SWF_warn = warnfunc;
}

void setSWFErrorFunction(void (*errorfunc)(char *msg, ...))
{
  SWF_error = errorfunc;
}

void SWF_assert(int c)
{
  if(!c)
    SWF_error("failed assertion");
}
