//
// Created by Ftofs on 2019/10/22.
//

#include <jni.h>
#include <android/log.h>

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JNIEXPORT jstring JNICALL
Java_com_example_ffmpegandroid_jni_Bridge_stringFromJNI(
        JNIEnv* env,
        jobject thiz) {
    slog("hello slog");
    return (*env)->NewStringUTF(env, "Hello from C");
}
