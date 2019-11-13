//
// Created by Ftofs on 2019/11/8.
//

#ifndef ANDROIDCAMERA_BRIDGE_H
#define ANDROIDCAMERA_BRIDGE_H

#include <stdbool.h>

void InitEncoder(const char *mp4Path, int width, int height);

void EncodeStart();

void EncodeBuffer(unsigned char *nv21Buffer);
int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *avPacket);
int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *avPacket);

typedef struct {
    char mp4Path[256];

    int width;

    int height;

    AVPacket avPacket;
    AVFormatContext *pFormatCtx;
    AVStream *pStream;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    uint8_t *pFrameBuffer;
    AVFrame *pFrame;

    //AVFrame PTS
    int index;

    bool transform;
} MP4EncoderInfo;

#endif //ANDROIDCAMERA_BRIDGE_H
