//
// Created by Ftofs on 2019/6/25.
//

#include <jni.h>
#include <android/log.h>
#include <arm_neon.h>

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


void neon()
{
    int16_t result[8];
    int8x8_t a = vdup_n_s8(121);
    int8x8_t b = vdup_n_s8(2);
    int16x8_t c;
    c = vmull_s8(a,b);
    vst1q_s16(result,c);
    for(int i=0;i<8;i++){
        slog("data[%d] is %d ", i, result[i]);
    }
}


JNIEXPORT jstring JNICALL
Java_com_example_lab_jni_JNITest_stringFromJNI(
        JNIEnv* env,
        jobject /* this */ thiz) {
    char * hello = "Hello from C++";
    neon();
    slog("hello slog");
    return (*env)->NewStringUTF(env, hello);
}
