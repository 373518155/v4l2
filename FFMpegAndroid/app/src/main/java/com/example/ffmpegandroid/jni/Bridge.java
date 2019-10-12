package com.example.ffmpegandroid.jni;

public class Bridge {
    static {
        System.loadLibrary("ffmpeg");
    }

    public native String hello();
}
