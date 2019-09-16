//
// Created by 37351 on 2019/9/8.
//



/*
 * linux v4l2摄像头应用层编程
 * https://www.jianshu.com/p/0ac427d267d4
 * 录制连续的YUYV视频，640x480
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#include "queue.h"
#include "slog.h"
#include "../common/common.h"


#define TRUE            (1)
#define FALSE           (0)

#define FILE_VIDEO      "/dev/video0"


#define FRAME_NUM       4

extern const char *gsLog;

// 摄像头设备文件描述符
int fd;

struct v4l2_buffer buf;

struct buffer
{
    void * start;
    unsigned int length;
    long long int timestamp;
} *buffers;

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

int v4l2_init()
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;
    struct v4l2_streamparm stream_para;

    //打开摄像头设备
    if ((fd = open(FILE_VIDEO, O_RDWR)) == -1)
    {
        printf("Error opening V4L interface\n");
        return FALSE;
    }

    //查询设备属性
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
        return FALSE;
    }
    else
    {
        printf("driver:\t\t%s\n",cap.driver);
        printf("card:\t\t%s\n",cap.card);
        printf("bus_info:\t%s\n",cap.bus_info);
        printf("version:\t%d\n",cap.version);
        printf("capabilities:\t%x\n",cap.capabilities);

        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
        {
            printf("Device %s: supports capture.\n",FILE_VIDEO);
        }

        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
        {
            printf("Device %s: supports streaming.\n",FILE_VIDEO);
        }
    }


    //显示所有支持帧格式
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index++;
    }

    //检查是否支持某帧格式
    struct v4l2_format fmt_test_rgb32;
    fmt_test_rgb32.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test_rgb32.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB32;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test_rgb32)==-1)
    {
        printf("not support format RGB32!\n");
    }
    else
    {
        printf("support format RGB32\n");
    }


    struct v4l2_format fmt_test_yuyv;
    fmt_test_yuyv.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test_yuyv.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB32;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test_yuyv)==-1)
    {
        printf("not support format YUYV!\n");
    }
    else
    {
        printf("support format YUYV\n");
    }


    //查看及设置当前格式
    printf("set fmt...\n");
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//yuv格式



    fmt.fmt.pix.height = IMAGEHEIGHT;
    fmt.fmt.pix.width = IMAGEWIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    printf("fmt.type:\t\t%d\n",fmt.type);
    printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
    printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
    printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        printf("Unable to set format\n");
        return FALSE;
    }

    printf("get fmt...\n");
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
    {
        printf("Unable to get format\n");
        return FALSE;
    }
    {
        printf("fmt.type:\t\t%d\n",fmt.type);
        printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
        printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
        printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
        printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    }

    //设置及查看帧速率，这里只能是30帧，就是1秒采集30张图
    memset(&stream_para, 0, sizeof(struct v4l2_streamparm));
    stream_para.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    stream_para.parm.capture.timeperframe.denominator = 30;
    stream_para.parm.capture.timeperframe.numerator = 1;

    if(ioctl(fd, VIDIOC_S_PARM, &stream_para) == -1)
    {
        printf("Unable to set frame rate\n");
        return FALSE;
    }
    if(ioctl(fd, VIDIOC_G_PARM, &stream_para) == -1)
    {
        printf("Unable to get frame rate\n");
        return FALSE;
    }
    {
        printf("numerator:%d\ndenominator:%d\n",stream_para.parm.capture.timeperframe.numerator,stream_para.parm.capture.timeperframe.denominator);
    }
    return TRUE;
}



int v4l2_mem_ops()
{
    unsigned int n_buffers;
    struct v4l2_requestbuffers req;

    //申请帧缓冲
    req.count=FRAME_NUM;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)
    {
        printf("request for buffers error\n");
        return FALSE;
    }

    // 申请用户空间的地址列
    buffers = malloc(req.count*sizeof (*buffers));
    if (!buffers)
    {
        printf ("out of memory!\n");
        return FALSE;
    }

    // 进行内存映射
    for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
    {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        //查询
        if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            printf("query buffer error\n");
            return FALSE;
        }

        //映射
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED)
        {
            printf("buffer map error\n");
            return FALSE;
        }
    }
    return TRUE;
}



int v4l2_frame_process(queue_t *q)
{
    unsigned int n_buffers;
    enum v4l2_buf_type type;

    long long int extra_time = 0;
    long long int cur_time = 0;
    long long int last_time = 0;

    //入队和开启采集
    for (n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
    {
        buf.index = n_buffers;
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);


    //出队，处理，写入yuv文件，入队，循环进行
    char filename[100];
    char timestamp[120];
    getTimestamp(timestamp);
    sprintf(filename, "vid_%s_%dx%d.yuyv", timestamp, IMAGEWIDTH, IMAGEHEIGHT);

    FrameData frameData;
    int frame_count = 0;
    int is_start = 0;
    while(1)
    {
        /*
         * 使用control.txt来控制录像的开始与结束: 如果有内容，则开始录像；如果文件为空，则结束录制
         */
        int control = getFileSize("control.txt");
        if (control > 0) {
            is_start = 1;
        } else { // 文件为空，表示不录制
            if (is_start) { // 如果已经开始，结束录制，投放flush指令
                // put a extra frameData with size 0 to flush the encoder
                frameData.size = 0;
                queue_put(q, (uint8_t *)&frameData, sizeof(FrameData));
                break;
            } else { // 未开始，跳过
                slog(gsLog, "未开始录制");
                usleep(100 * 1000);
                continue;
            }
        }

        for(n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
        {
            //出队
            buf.index = n_buffers;
            ioctl(fd, VIDIOC_DQBUF, &buf);

            //查看采集数据的时间戳之差，单位为微妙
            buffers[n_buffers].timestamp = buf.timestamp.tv_sec*1000000+buf.timestamp.tv_usec;
            cur_time = buffers[n_buffers].timestamp;
            extra_time = cur_time - last_time;
            last_time = cur_time;
            printf("time_deta:%lld\n\n",extra_time);
            printf("buf_len:%d\n",buffers[n_buffers].length);

            // 由数据提供方分配内存，由数据使用方释放内存，因为内容数据并没有存放到队列中去，所以需要数据使用方用完后，释放内存
            frameData.size = FRAME_SIZE;
            frameData.data = malloc(FRAME_SIZE);
            if (frameData.data == NULL) {
                slog(gsLog, "Error!");
                return -1;
            }
            memcpy(frameData.data, buffers[n_buffers].start, FRAME_SIZE);
            appenddata(filename, buffers[n_buffers].start, FRAME_SIZE); // 同时记录到yuyv文件中

            queue_put(q, (uint8_t *)&frameData, sizeof(FrameData));

            slog(gsLog, "Process %d frames data", frame_count);

            //入队循环
            ioctl(fd, VIDIOC_QBUF, &buf);
            frame_count++;
        }
    }
    return TRUE;
}




int v4l2_release()
{
    unsigned int n_buffers;
    enum v4l2_buf_type type;

    //关闭流
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMON, &type);

    //关闭内存映射
    for(n_buffers=0;n_buffers<FRAME_NUM;n_buffers++)
    {
        munmap(buffers[n_buffers].start,buffers[n_buffers].length);
    }

    //释放自己申请的内存
    free(buffers);

    //关闭设备
    close(fd);
    return TRUE;
}

int capture(queue_t *q)
{
    v4l2_init();
    slog(gsLog, "init....");

    v4l2_mem_ops();
    slog(gsLog, "malloc....");

    v4l2_frame_process(q);
    slog(gsLog, "process....");

    v4l2_release();
    slog(gsLog, "release");

    return TRUE;
}

