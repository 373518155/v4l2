//
// Created by Ftofs on 2019/9/16.
//

#include "common.h"


/**
 *
 * @param timestamp
 * @return
 */


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
