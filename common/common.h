//
// Created by Ftofs on 2019/9/16.
//

#ifndef V4L2_COMMON_H
#define V4L2_COMMON_H

const char *getTimestamp(const char *timestamp, int format);
int readdata(const char *filename, int offset, void *data, int len);
int writedata(const char *filename, int offset, const void *data, int len);
int appenddata(const char *filename, const void *data, int len);

/* 文件不存在时返回-1 */
int getFileSize(const char *path);

#define MAX_KEY_NAME_LEN 64
#define MAX_VALUE_CONTENT_SIZE 4096
int getPropConfig(const char *cfgFilePath, const char *key, char value[MAX_VALUE_CONTENT_SIZE + 1]/* out */);
int getIntPropConfig(const char *cfgFilePath, const char *key, int *value/* out */);

void LTrim(char *str);
void RTrim(char *str);
void LRTrim(char *str);

#endif //V4L2_COMMON_H
