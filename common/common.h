//
// Created by Ftofs on 2019/9/16.
//

#ifndef V4L2_COMMON_H
#define V4L2_COMMON_H

const char *getTimestamp(const char *timestamp);
int appenddata(const char *filename, const void *data, int len);

#endif //V4L2_COMMON_H
