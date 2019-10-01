//
// Created by Ftofs on 2019/9/16.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/stat.h>
#include "common.h"



/**
 * 获取时间戳字符串
 * @param timestamp 存放时间戳结果,由调用方负责内存
 * @param format 格式:
 *              1 -- [2019-09-16 20:50:17.081]
 *              2 -- [2019-09-16 20:50:17]
 *              3 -- [20190916205017081]
 *              4 -- [20190916205017]
 * @return 返回时间戳
 */
const char *getTimestamp(const char *timestamp, int format) {
    /* get system time */
    struct timeval tms;
    char tstr[100] = "";
    timerclear(&tms);
    gettimeofday(&tms, NULL);
    if (format == 1 || format == 2) {
        strftime(tstr, 100, "%Y-%m-%d %H:%M:%S", localtime(&tms.tv_sec));
    } else if (format == 3 || format == 4) {
        strftime(tstr, 100, "%Y%m%d%H%M%S", localtime(&tms.tv_sec));
    }

    if (format == 1) {
        sprintf(timestamp, "%s.%03ld", tstr, tms.tv_usec / 1000); /* tv_usec是微秒，除以1000转换为毫秒 */
    } else if (format == 3) {
        sprintf(timestamp, "%s%03ld", tstr, tms.tv_usec / 1000); /* tv_usec是微秒，除以1000转换为毫秒 */
    }

    /* end of get system time */

    return timestamp;
}

/*
 * 从文件filename的offset处读取len字节，存到data中，返回已读取数据的字节数，如果失败，返回负数
 * */
int readdata(const char *filename, int offset, void *data, int len)
{
    if (len <= 0)
    {
        return len;
    }

    FILE *fp = fopen(filename, "rb");

    if (!fp)
    {
        return -1;
    }

    if (0 != fseek(fp, offset, SEEK_SET))
    {
        fclose(fp);
        return -2;
    }

    if (1 != fread(data, len, 1, fp))
    {
        fclose(fp);
        return -3;
    }

    fclose(fp);
    return len;
}

/*
 * write [len] bytes [data] to [filename] at [offset], if the spcified file is not exist, this function will create it
 * return [len] on success, otherwise return a negative integer
 * */
int writedata(const char *filename, int offset, const void *data, int len)
{
    if (len <= 0)
    {
        return len;
    }


    /* check weather the specified file exists */
    FILE *fp = fopen(filename, "rb");
    if (!fp)/* the file not exists, create it */
    {
        fp = fopen(filename, "wb");
        if (!fp)
        {
            return -1; /* create file failed */
        }
        fclose(fp);
    }
    else
    {
        fclose(fp);
    }


    /* open for writing data */
    fp = fopen(filename, "rb+");
    if (!fp)
    {
        return -2;
    }


    if (0 != fseek(fp, offset, SEEK_SET))
    {
        fclose(fp);
        return -3;
    }


    if (1 != fwrite(data, len, 1, fp))
    {
        fclose(fp);
        return -4;
    }

    fclose(fp);
    return len;
}

/*
 *  * append [len] bytes [data] to [filename] at the end of file, if the spcified file is not exist, this function will create it
 *  * return [len] on success, otherwise return a negative integer
 *  * */

int appenddata(const char *filename, const void *data, int len)
{
    if (len <= 0)
    {
        return len;
    }


    /* open for writing data */
    FILE *fp = fopen(filename, "ab");
    if (!fp)
    {
        return -1;
    }


    if (1 != fwrite(data, len, 1, fp))
    {
        fclose(fp);
        return -2;
    }

    fclose(fp);
    return len;

}


/* 文件不存在时返回-1 */
int getFileSize(const char *path)
{
    int filesize = -1;
    struct stat statbuff;
    if (stat(path, &statbuff) < 0)
    {
        return filesize;
    }
    else
    {
        filesize = statbuff.st_size;
    }
    return filesize;
}

/* 获取log4j格式的propery配置，return 0 on success */
int getPropConfig(const char *cfgFilePath, const char *key, char value[MAX_VALUE_CONTENT_SIZE + 1]/* out */)
{
    int nRet = 0;
    FILE *fp = fopen(cfgFilePath, "r");
    if (!fp)
    {
        nRet = -2; /* error, file not found! */
        goto GETPROPCONFIG_EXIT1;
    }

    /* line buffer */
    char buffer[MAX_KEY_NAME_LEN + MAX_VALUE_CONTENT_SIZE + 200] = "";

    /* 定位出指定的key */
    int keyname_len = strlen(key);
    while (1)
    {
        if (fgets(buffer, sizeof(buffer), fp))
        {
            LRTrim(buffer); /* trim the white-spaces */
            if ('#' == buffer[0]) /* comment, skip it */
            {
                continue;
            }
            if (0 == strncmp(key, buffer, keyname_len))
            {
                break; /* we have found the specified key */
            }
        }
        else /* error, reach end of file, but cannot find the specified key */
        {
            nRet = -5;
            goto GETPROPCONFIG_EXIT2;
        }
    }

    /* pos为等号的位置指针 */
    char *pos = strchr(buffer, '=');
    if (pos)
    {
        ++pos; /* 移动到等号后面 */
        LRTrim(pos);
        int newlen = strlen(pos);
        memcpy(value, pos, newlen + 1); /* 加上结尾那个NULL */
    }
    else
    {
        nRet = -6; /* 找不到等号 */
        goto GETPROPCONFIG_EXIT2;
    }

    GETPROPCONFIG_EXIT2:
    fclose(fp);
    GETPROPCONFIG_EXIT1:
    return nRet;
}

/* getPropConfig的int版, return 0 on success */
int getIntPropConfig(const char *cfgFilePath, const char *key, int *value/* out */)
{
    char val[MAX_VALUE_CONTENT_SIZE + 1] = "";
    int rc = getPropConfig(cfgFilePath, key, val);
    if (0 == rc)
    {
        *value = atoi(val);
    }
    return rc;
}


/* 对str删除左边的空格，tab键，回车，换行字符 */
void LTrim(char *str)
{
    int len = strlen(str);
    if (len < 1)
    {
        return;
    }
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        if (0x20 == str[i] ||
            '\t' == str[i] ||
            '\r' == str[i] ||
            '\n' == str[i]
                )
        {
            continue;
        }
        else
        {
            break;
        }
    }
    int j = i;
    for (j = i; j < len; ++j)
    {
        str[j - i] = str[j];
    }
    str[len - i] = '\0';
}

/* 对str删除左边的空格，tab键，回车，换行字符 */
void RTrim(char *str)
{
    int len = strlen(str);
    if (len < 1)
    {
        return;
    }
    int i = len - 1;
    for (i = len - 1; i >= 0; --i)
    {
        if (0x20 == str[i] ||
            '\t' == str[i] ||
            '\r' == str[i] ||
            '\n' == str[i]
                )
        {
            str[i] = '\0';
        }
        else
        {
            break;
        }
    }
}

/* 对str删除左右两边的空格，tab键，回车，换行字符 */
void LRTrim(char *str)
{
    LTrim(str);
    RTrim(str);
}


