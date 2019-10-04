package com.example.lab.jni;

public class JNITest {
    static {
        System.loadLibrary("native-libc");
    }
    public native String stringFromJNI();
}
