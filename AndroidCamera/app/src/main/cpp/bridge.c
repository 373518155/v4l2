//
// Created by Ftofs on 2019/10/22.
//

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <unistd.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"
#include "libavutil/imgutils.h"

#include "bridge.h"

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


MP4EncoderInfo mp4EncoderInfo;

/*
 * 测试
 */
JNIEXPORT jstring JNICALL
Java_com_example_lab_jni_Bridge_stringFromJNI(
        JNIEnv* env,
        jobject thiz) {
    slog("hello slog");
    return (*env)->NewStringUTF(env, "Hello from C");
}

/*
 * 获取当前工作目录
 */
JNIEXPORT jstring JNICALL
Java_com_example_lab_jni_Bridge_getCWD(
        JNIEnv* env,
        jobject thiz) {
    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    return (*env)->NewStringUTF(env, cwd);
}


/*
 * 获取支持的封装格式信息
 */
JNIEXPORT jstring JNICALL
Java_com_example_lab_jni_Bridge_getAVFormatInfo
        (JNIEnv * env, jobject obj){

    char info[40000] = { 0 };

    av_register_all();

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    //input
    while(if_temp!=NULL){
        sprintf(info, "%s[in ][%10s]\n", info, if_temp->name);
        if_temp=if_temp->next;
    }
    //output
    while (of_temp != NULL){
        sprintf(info, "%s[out][%10s]\n", info, of_temp->name);
        of_temp = of_temp->next;
    }
    slog("%s", info);
    return (*env)->NewStringUTF(env, info);
}

/*
 * 获取编解码器信息
 */
JNIEXPORT jstring JNICALL
Java_com_example_lab_jni_Bridge_getAVCodecInfo
        (JNIEnv * env, jobject obj){

    char info[40000] = { 0 };

    av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while(c_temp!=NULL){
        if (c_temp->decode!=NULL){
            sprintf(info, "%s[dec]", info);
        }
        else{
            sprintf(info, "%s[enc]", info);
        }
        switch (c_temp->type){
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s[video]", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s[audio]", info);
                break;
            default:
                sprintf(info, "%s[other]", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);

        c_temp=c_temp->next;
    }
    return (*env)->NewStringUTF(env, info);
}

/**
 * 根据编码器名称判断编码器是否存在
 * @param env
 * @param obj
 * @param encoderName_ 编码器名称，比如  libx264、libfdk_aac
 * @return
 */
JNIEXPORT jboolean JNICALL
Java_com_example_lab_jni_Bridge_isEncoderExists(JNIEnv *env, jobject instance,
                                                jstring encoderName_) {
    const char *encoderName = (*env)->GetStringUTFChars(env, encoderName_, 0);

    // TODO
    jboolean result;
    avcodec_register_all();

    AVCodec* avCodec = avcodec_find_encoder_by_name(encoderName);
    if (avCodec) {
        /*
        printf("Found!\n");
        printf("name[%s]\n", avCodec->name);
        printf("long_name[%s]\n", avCodec->long_name);
         */
        result = JNI_TRUE;
    } else {
        // printf("Error!\n");
        result = JNI_FALSE;
    }

    (*env)->ReleaseStringUTFChars(env, encoderName_, encoderName);
    return result;
}



/**
 * 编码开始
 * @param env
 * @param instance
 * @param mp4Path_
 * @param width
 * @param height
 */
JNIEXPORT void JNICALL
Java_com_example_lab_jni_Bridge_encodeMP4Start(JNIEnv *env, jobject instance, jstring mp4Path_,
                                               jint width, jint height) {
    const char *mp4Path = (*env)->GetStringUTFChars(env, mp4Path_, 0);

    // TODO

    InitEncoder(mp4Path, width, height);
    EncodeStart();

    (*env)->ReleaseStringUTFChars(env, mp4Path_, mp4Path);
}




/**
 * 编码结束
 * @param env
 * @param instance
 */
JNIEXPORT void JNICALL
Java_com_example_lab_jni_Bridge_encodeMP4Stop(JNIEnv *env, jobject instance) {

    // TODO
    //标记转换结束
    mp4EncoderInfo.transform = false;

    int result = EncodeFrame(mp4EncoderInfo.pCodecCtx, NULL, &mp4EncoderInfo.avPacket);
    if (result >= 0) {
        //封装文件尾
        av_write_trailer(mp4EncoderInfo.pFormatCtx);
        //释放内存
        if (mp4EncoderInfo.pCodecCtx != NULL) {
            avcodec_close(mp4EncoderInfo.pCodecCtx);
            avcodec_free_context(&mp4EncoderInfo.pCodecCtx);
            mp4EncoderInfo.pCodecCtx = NULL;
        }
        if (mp4EncoderInfo.pFrame != NULL) {
            av_free(mp4EncoderInfo.pFrame);
            mp4EncoderInfo.pFrame = NULL;
        }
        if (mp4EncoderInfo.pFrameBuffer != NULL) {
            av_free(mp4EncoderInfo.pFrameBuffer);
            mp4EncoderInfo.pFrameBuffer = NULL;
        }
        if (mp4EncoderInfo.pFormatCtx != NULL) {
            avio_close(mp4EncoderInfo.pFormatCtx->pb);
            avformat_free_context(mp4EncoderInfo.pFormatCtx);
            mp4EncoderInfo.pFormatCtx = NULL;
        }
    }

}


/**
 * 处理相机回调的预览数据
 * @param env
 * @param instance
 * @param yuvData_
 * @param width
 * @param height
 */
JNIEXPORT void JNICALL
Java_com_example_lab_jni_Bridge_onPreviewFrame(JNIEnv *env, jobject instance, jbyteArray yuvArray,
                                               jint width, jint height) {
    jbyte *yuv420Buffer = (*env)->GetByteArrayElements(env, yuvArray, NULL);

    // TODO
    EncodeBuffer((unsigned char *) yuv420Buffer);

    (*env)->ReleaseByteArrayElements(env, yuvArray, yuv420Buffer, 0);
}

void InitEncoder(const char *mp4Path, int width, int height) {
    strcpy(mp4EncoderInfo.mp4Path, mp4Path);
    mp4EncoderInfo.width = width;
    mp4EncoderInfo.height = height;
}



void EncodeStart() {
    //1. 注册所有组件
    av_register_all();
    //2. 初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&mp4EncoderInfo.pFormatCtx, NULL, NULL, mp4EncoderInfo.mp4Path);

    //3. 打开待输出的视频文件
    if (avio_open(&mp4EncoderInfo.pFormatCtx->pb, mp4EncoderInfo.mp4Path, AVIO_FLAG_READ_WRITE)) {
        slog("open output file failed");
        return;
    }
    //4. 初始化视频码流
    mp4EncoderInfo.pStream = avformat_new_stream(mp4EncoderInfo.pFormatCtx, NULL);
    if (mp4EncoderInfo.pStream == NULL) {
        slog("allocating output stream failed");
        return;
    }
    //5. 寻找编码器并打开编码器
    mp4EncoderInfo.pCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if (!mp4EncoderInfo.pCodec) {
        slog("could not find encoder");
        return;
    }

    //6. 分配编码器并设置参数
    mp4EncoderInfo.pCodecCtx = avcodec_alloc_context3(mp4EncoderInfo.pCodec);
    mp4EncoderInfo.pCodecCtx->codec_id = mp4EncoderInfo.pCodec->id;
    mp4EncoderInfo.pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    mp4EncoderInfo.pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    mp4EncoderInfo.pCodecCtx->width = mp4EncoderInfo.height;
    mp4EncoderInfo.pCodecCtx->height = mp4EncoderInfo.width;
    mp4EncoderInfo.pCodecCtx->time_base.num = 1;
    mp4EncoderInfo.pCodecCtx->time_base.den = 25;
    mp4EncoderInfo.pCodecCtx->bit_rate = 400000;
    mp4EncoderInfo.pCodecCtx->gop_size = 12;


    //将AVCodecContext的成员复制到AVCodecParameters结构体
    avcodec_parameters_from_context(mp4EncoderInfo.pStream->codecpar, mp4EncoderInfo.pCodecCtx);
    AVRational avRational = {1, 25};
    av_stream_set_r_frame_rate(mp4EncoderInfo.pStream, avRational);

    //7. 打开编码器
    if (avcodec_open2(mp4EncoderInfo.pCodecCtx, mp4EncoderInfo.pCodec, NULL) < 0) {
        slog("open encoder fail!");
        return;
    }

    //输出格式信息
    av_dump_format(mp4EncoderInfo.pFormatCtx, 0, mp4EncoderInfo.mp4Path, 1);

    //初始化帧
    mp4EncoderInfo.pFrame = av_frame_alloc();
    mp4EncoderInfo.pFrame->width = mp4EncoderInfo.pCodecCtx->width;
    mp4EncoderInfo.pFrame->height = mp4EncoderInfo.pCodecCtx->height;
    mp4EncoderInfo.pFrame->format = mp4EncoderInfo.pCodecCtx->pix_fmt;
    int bufferSize = av_image_get_buffer_size(mp4EncoderInfo.pCodecCtx->pix_fmt, mp4EncoderInfo.pCodecCtx->width,
                                              mp4EncoderInfo.pCodecCtx->height, 1);
    mp4EncoderInfo.pFrameBuffer = (uint8_t *) av_malloc(bufferSize);
    av_image_fill_arrays(mp4EncoderInfo.pFrame->data, mp4EncoderInfo.pFrame->linesize, mp4EncoderInfo.pFrameBuffer, mp4EncoderInfo.pCodecCtx->pix_fmt,
                         mp4EncoderInfo.pCodecCtx->width, mp4EncoderInfo.pCodecCtx->height, 1);

    AVDictionary *opt = 0;
    //H.264
    if (mp4EncoderInfo.pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set_int(&opt, "video_track_timescale", 25, 0);
        av_dict_set(&opt, "preset", "slow", 0);
        av_dict_set(&opt, "tune", "zerolatency", 0);
    }
    //8. 写文件头
    avformat_write_header(mp4EncoderInfo.pFormatCtx, &opt);

    //创建已编码帧
    av_new_packet(&mp4EncoderInfo.avPacket, bufferSize * 3);

    //标记正在转换
    mp4EncoderInfo.transform = true;

}

void EncodeBuffer(unsigned char *nv21Buffer) {

    uint8_t *i420_y = mp4EncoderInfo.pFrameBuffer;
    uint8_t *i420_u = mp4EncoderInfo.pFrameBuffer + mp4EncoderInfo.width * mp4EncoderInfo.height;
    uint8_t *i420_v = mp4EncoderInfo.pFrameBuffer + mp4EncoderInfo.width * mp4EncoderInfo.height * 5 / 4;

    //NV21转I420
    libyuv::ConvertToI420(nv21Buffer, mp4EncoderInfo.width * mp4EncoderInfo.height, i420_y, mp4EncoderInfo.height, i420_u, mp4EncoderInfo.height / 2, i420_v,
                          mp4EncoderInfo.height / 2, 0, 0, mp4EncoderInfo.width, mp4EncoderInfo.height, mp4EncoderInfo.width, mp4EncoderInfo.height, libyuv::kRotate270,
                          libyuv::FOURCC_NV21);

    mp4EncoderInfo.pFrame->data[0] = i420_y;
    mp4EncoderInfo.pFrame->data[1] = i420_u;
    mp4EncoderInfo.pFrame->data[2] = i420_v;

    //AVFrame PTS
    mp4EncoderInfo.pFrame->pts = mp4EncoderInfo.index++;

    //编码数据
    EncodeFrame(mp4EncoderInfo.pCodecCtx, mp4EncoderInfo.pFrame, &mp4EncoderInfo.avPacket);
}

int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *avPacket) {
    int ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret < 0) {
        //failed to send frame for encoding
        return -1;
    }
    while (!ret) {
        ret = avcodec_receive_packet(pCodecCtx, avPacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            //error during encoding
            return -1;
        }
        //printf("Write frame %d, size=%d\n", avPacket->pts, avPacket->size);
        avPacket->stream_index = mp4EncoderInfo.pStream->index;
        av_packet_rescale_ts(avPacket, pCodecCtx->time_base, mp4EncoderInfo.pStream->time_base);
        avPacket->pos = -1;
        av_interleaved_write_frame(mp4EncoderInfo.pFormatCtx, avPacket);
        av_packet_unref(avPacket);
    }
    return 0;
}

