//
// Created by Ftofs on 2019/9/5.
//

#ifndef V4L2_SLOG_H
#define V4L2_SLOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h> /* isprint */
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

extern const char *gsLog; /* gsLog不需要带扩展名，一般定义为进程的名称或Sub的名称 */
int elog(const char *logname, const char *srcfile, int srcline, const char *format, ...);
int hexlog(const char *logname, const char *srcfile, int srcline, const void *buffer, int len);

#define slog(gsLog, ...) elog(gsLog, __FILE__, __LINE__, __VA_ARGS__)  /* simple log */
#define hlog(gsLog, buffer, len) hexlog(gsLog, __FILE__, __LINE__, buffer, len)  /* hex log */

#endif //V4L2_SLOG_H
