package com.example.lab;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class CameraActivity extends AppCompatActivity implements View.OnClickListener {
    CameraPreview cameraPreview;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        Util.enterFullScreen(this);

        findViewById(R.id.btn_info).setOnClickListener(this);
        findViewById(R.id.btn_shutter).setOnClickListener(this);
        cameraPreview = findViewById(R.id.camera_preview);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();

        Util.exitFullScreen(this);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();

        if (id == R.id.btn_info) {
            cameraPreview.getCameraInfo();
        } else if (id == R.id.btn_shutter) {
            cameraPreview.takePicture();
        }
    }
}
