package com.example.lab.jni;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    public native String stringFromJNI();
    public native String getCWD();
    public native String getAVFormatInfo();
    public native String getAVCodecInfo();
    public native boolean isEncoderExists(String encoderName);

    public native void encodeMP4Start(String mp4Path, int width, int height);

    public native void encodeMP4Stop();

    public native void onPreviewFrame(byte[] yuvData, int width, int height);
}

