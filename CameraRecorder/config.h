//
// Created by 37351 on 2019/9/8.
//

#ifndef GIT_CONFIG_H
#define GIT_CONFIG_H


// 宽度
#define IMAGEWIDTH      640
// 高度
#define IMAGEHEIGHT     480
// 每个YUYV帧的大小
#define FRAME_SIZE (IMAGEWIDTH * IMAGEHEIGHT * 2)
// 帧率
#define FPS             30

/**
 * YUYV Frame Data
 */
typedef struct {
    int size;  // data size
    unsigned char *data;
} FrameData;


#endif //GIT_CONFIG_H
