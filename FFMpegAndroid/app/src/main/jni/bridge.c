//
// Created by Ftofs on 2019/10/22.
//

#include <jni.h>
#include <android/log.h>
#include <stdio.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_jni_Bridge_stringFromJNI(
        JNIEnv* env,
        jobject thiz) {
    slog("hello slog");
    return (*env)->NewStringUTF(env, "Hello from C");
}


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

