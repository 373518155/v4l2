//
// Created by Ftofs on 2019/10/29.
//

#ifndef TRIDENT_SLOG_H
#define TRIDENT_SLOG_H


int elog(const char *logname, const char *srcfile, int srcline, const char *format, ...);
int hexlog(const char *logname, const char *srcfile, int srcline, const void *buffer, int len);

#define slog(gsLog, ...) elog(gsLog, __FILE__, __LINE__, __VA_ARGS__)  /* simple log */
#define hlog(gsLog, buffer, len) hexlog(gsLog, __FILE__, __LINE__, buffer, len)  /* hex log */

#endif //TRIDENT_SLOG_H
