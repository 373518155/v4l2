package com.example.ffmpegandroid.jni;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    public native String stringFromJNI();
    public native String getAVFormatInfo();
}
