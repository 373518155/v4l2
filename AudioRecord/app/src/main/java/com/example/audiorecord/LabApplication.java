package com.example.audiorecord;

import android.app.Application;

public class LabApplication extends Application {
    private static LabApplication instance;
    public static LabApplication getInstance() {
        return instance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        
        instance = this;
    }
}
