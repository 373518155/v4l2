package com.example.lab;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    public native String stringFromJNI();
}
