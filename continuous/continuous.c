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


#define TRUE            (1)
#define FALSE           (0)

#define FILE_VIDEO      "/dev/video0"

#define IMAGEWIDTH      640
#define IMAGEHEIGHT     480
#define FPS             30
#define FRAME_NUM       4




int fd;
int duration;  // 拍摄时长（秒）

struct v4l2_buffer buf;

struct buffer
{
    void * start;
    unsigned int length;
    long long int timestamp;
} *buffers;


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



int v4l2_frame_process()
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
    int loop = 0;
    int max_loop = duration * FPS / FRAME_NUM;
    int total_frame_count = max_loop * FRAME_NUM;   // total frame count
    char filename[100];
    time_t now = time(0);
    sprintf(filename, "vid_%ld_%dx%d_%dframes.yuyv", now, IMAGEWIDTH, IMAGEHEIGHT, total_frame_count);
    printf("filename[%s]\n", filename);

    int frame_count = 0;
    while(loop < max_loop)
    {
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

            //处理数据只是简单写入文件，名字以loop的次数和帧缓冲数目有关
            printf("grab image data OK\n");

            appenddata(filename, buffers[n_buffers].start, IMAGEWIDTH * IMAGEHEIGHT * 2);

            printf("save frame %d OK\n", frame_count);

            //入队循环
            ioctl(fd, VIDIOC_QBUF, &buf);
            frame_count++;
        }

        loop++;
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

int main(int argc, char const *argv[])
{
    if (argc == 2) {
        duration = atoi(argv[1]);
    } else {
        printf("usage: %s <duration>\n\tduration seconds\n", argv[0]);
        return 0;
    }

    printf("begin....\n");
    sleep(5);

    v4l2_init();
    printf("init....\n");
    sleep(5);

    v4l2_mem_ops();
    printf("malloc....\n");
    sleep(5);

    v4l2_frame_process();
    printf("process....\n");
    sleep(5);

    v4l2_release();
    printf("release\n");
    sleep(5);

    return TRUE;
}

