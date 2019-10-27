package com.example.ffmpegandroid.jni;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    public native String stringFromJNI();
    public native String getCWD();
    public native String getAVFormatInfo();
    public native String getAVCodecInfo();
    public native boolean isEncoderExists(String encoderName);
}
