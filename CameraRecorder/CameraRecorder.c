/*
 * 摄像头抓取YUYV数据，通过生产者-消费者模型，将YUYV数据传递给H264编码线程，进行编码
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "stdint.h"
#include "x264.h"
#include "queue.h"
#include "slog.h"


#define WIDTH 640
#define HEIGHT 480

#define FRAME_NUM 480
#define FRAME_SIZE (WIDTH * HEIGHT * 2)
#define QUEUE_BUFFER_SIZE (16 * getpagesize())

unsigned char in_buf[FRAME_SIZE];  // 读取每个YUYV帧的BUFFER

const char *gsLog = "CameraRecorder";

/**
 * YUYV Frame Data
 */
typedef struct {
    int size;  // data size
    unsigned char *data;
} FrameData;


int encodeYUYV(x264_t* pHandle, x264_param_t* pParam, FrameData *pFrameData, FILE *fp_dst,
        x264_picture_t* pPic_in, x264_picture_t* pPic_out, int pts)
{
    int dataSize = pFrameData->size;
    int ret;
    int y_size;
    int i,j;

    unsigned char *y_plane;
    unsigned char *u_plane;
    unsigned char *v_plane;

    x264_nal_t* pNals = NULL;
    int iNal   = 0;
    y_size = pParam->i_width * pParam->i_height;
    if (dataSize > 0) {
        /*
         * YUYV的存放顺序是  YUYV YUYV YUYV ... 这种packed格式
         * 数据是交错存储的，因此需要把他们分离出来单独存储
         * 参考  https://blog.csdn.net/zgyulongfei/article/details/7526327
         */
        y_plane = pPic_in->img.plane[0];
        u_plane = pPic_in->img.plane[1];
        v_plane = pPic_in->img.plane[2];
        int is_y = 1, is_u = 1;
        int y_index = 0, u_index = 0, v_index = 0;
        for (j = 0; j < FRAME_SIZE; j++) {
            unsigned char val = pFrameData->data[j];
            if (is_y) {
                *(y_plane + y_index) = val;
                ++y_index;
                is_y = 0;
            } else {
                if (is_u) {
                    *(u_plane + u_index) = val;
                    ++u_index;
                    is_u = 0;
                } else {
                    *(v_plane + v_index) = val;
                    ++v_index;
                    is_u = 1;
                }
                is_y = 1;
            }
        }

        pPic_in->i_pts = pts;

        ret = x264_encoder_encode(pHandle, &pNals, &iNal, pPic_in, pPic_out);
        if (ret< 0){
            printf("Error.\n");
            return -1;
        }

        printf("Succeed encode frame: %5d\n", pts);

        for ( j = 0; j < iNal; ++j){
            fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fp_dst);
        }
    } else { // if dataSize == 0, means finish,  flush encoder
        i=0;
        //
        while(1){
            ret = x264_encoder_encode(pHandle, &pNals, &iNal, NULL, pPic_out);
            if(ret==0){
                break;
            }
            printf("Flush 1 frame.\n");
            for (j = 0; j < iNal; ++j){
                fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fp_dst);
            }
            i++;
        }
    }

    return 0;
}


void *consumer_loop(void *arg) {
    slog(gsLog, "consumer_loop start...");
    FILE* fp_dst = fopen("output.h264", "wb");
    //Check
    if(fp_dst==NULL){
        printf("Error open files.\n");
        return (void *)-1;
    }

    int csp=X264_CSP_I422;  /* yuv 4:2:2 planar */
    int width=WIDTH,height=HEIGHT;


    x264_t* pHandle   = NULL;
    x264_picture_t* pPic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    x264_picture_t* pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    x264_param_t* pParam = (x264_param_t*)malloc(sizeof(x264_param_t));

    x264_param_default(pParam);
    pParam->i_width   = width;
    pParam->i_height  = height;

    pParam->i_csp=csp;
    x264_param_apply_profile(pParam, x264_profile_names[5]);

    pHandle = x264_encoder_open(pParam);

    x264_picture_init(pPic_out);
    x264_picture_alloc(pPic_in, csp, pParam->i_width, pParam->i_height);

    queue_t *q = (queue_t *) arg;
    size_t count = 0;
    size_t len = 0;

    FrameData frameData;
    while (1) {
        len = queue_get(q, (uint8_t *) &frameData, sizeof(FrameData));

        if (encodeYUYV(pHandle, pParam, &frameData, fp_dst, pPic_in, pPic_out, count) != 0) {
            slog(gsLog, "Error!");
            break;
        }

        if (frameData.size == 0) {
            break;
        }
        count++;
    }

    x264_picture_clean(pPic_in);
    x264_encoder_close(pHandle);
    pHandle = NULL;

    free(pPic_in);
    free(pPic_out);
    free(pParam);

    fclose(fp_dst);
    slog(gsLog, "consumer_loop end...");
    return (void *) count;
}


int main() {
    slog(gsLog, "main start...");


    FILE* fp_src  = fopen("./vid_1565629469_640x480_480frames.yuyv", "rb");
    if (fp_src == NULL) {
        printf("Error open files.\n");
        return -1;
    }
    queue_t q;
    queue_init(&q, QUEUE_BUFFER_SIZE);

    pthread_t consumer;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&consumer, &attr, &consumer_loop, (void *)&q);

    int i = 0;
    FrameData *pFrameData;
    for (i = 0; i < FRAME_NUM; i++) {
        // 读取一帧数据
        slog(gsLog, "Process %d frames data", i);
        fread(in_buf, FRAME_SIZE, 1, fp_src);
        pFrameData = malloc(sizeof(FrameData));

        slog(gsLog, "here");
        if (pFrameData == NULL) {
            slog(gsLog, "Error!");
            return -1;
        }
        
        pFrameData->data = malloc(FRAME_SIZE);
        if (pFrameData->data == NULL) {
            slog(gsLog, "Error!");
            return -1;
        }

        pFrameData->size = FRAME_SIZE;
        memcpy(pFrameData->data, in_buf, FRAME_SIZE);
        queue_put(&q, (uint8_t *)pFrameData, sizeof(FrameData));
    }

    // put a extra frameData with size 0 to flush the encoder
    pFrameData->size = 0;
    queue_put(&q, (uint8_t *)pFrameData, sizeof(FrameData));

    intptr_t result;
    pthread_join(consumer, (void **) &result);

    pthread_attr_destroy(&attr);
    queue_destroy(&q);

    fclose(fp_src);


    slog(gsLog, "main end...");
    return 0;
}

