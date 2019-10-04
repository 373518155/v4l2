package com.example.lab;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.example.lab.jni.JNITest;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionUtil.requestStoragePermission(this);

        SLog.info("JNIString[%s]", new JNITest().stringFromJNI());
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
    }
}
