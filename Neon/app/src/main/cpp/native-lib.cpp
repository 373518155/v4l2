#include <jni.h>
#include <string>
#include <android/log.h>

#define TAG "SLog"
#define slog(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_ftofs_ndklab_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    slog("hello slog");
    return env->NewStringUTF(hello.c_str());
}
