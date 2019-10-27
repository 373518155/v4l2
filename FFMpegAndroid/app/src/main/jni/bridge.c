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

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

/*
 * 测试
 */
JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_jni_Bridge_stringFromJNI(
        JNIEnv* env,
        jobject thiz) {
    slog("hello slog");
    return (*env)->NewStringUTF(env, "Hello from C");
}

/*
 * 获取当前工作目录
 */
JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_jni_Bridge_getCWD(
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
Java_com_example_ffmpegandroid_jni_Bridge_getAVFormatInfo
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
Java_com_example_ffmpegandroid_jni_Bridge_getAVCodecInfo
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
 * @param jstrEncoderName 编码器名称，比如  libx264、libfdk_aac
 * @return
 */
JNIEXPORT jboolean JNICALL
Java_com_example_ffmpegandroid_jni_Bridge_isEncoderExists(JNIEnv * env, jobject obj, jstring jstrEncoderName) {
    const char *encoderName = (*env)->GetStringUTFChars(env, jstrEncoderName, 0);

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


    (*env)->ReleaseStringUTFChars(env, jstrEncoderName, encoderName);

    return result;
}





