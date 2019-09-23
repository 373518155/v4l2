package com.example.lab;

import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.yanzhenjie.permission.Permission;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    Button btnRequestStoragePermission;
    Button btnRequestCameraPermission;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnRequestStoragePermission = findViewById(R.id.btn_request_storage_permission);
        btnRequestCameraPermission = findViewById(R.id.btn_request_camera_permission);
        if (PermissionUtil.hasStoragePermission()) {
            btnRequestStoragePermission.setVisibility(View.GONE);
        } else {
            btnRequestStoragePermission.setOnClickListener(this);
        }
        if (PermissionUtil.hasCameraPermission()) {
            btnRequestCameraPermission.setVisibility(View.GONE);
        } else {
            btnRequestCameraPermission.setOnClickListener(this);
        }

        findViewById(R.id.btn_capture).setOnClickListener(this);
        findViewById(R.id.btn_get_preview_resolution).setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        if (id == R.id.btn_request_storage_permission) {
            PermissionUtil.requestStoragePermission(this);
        } else if (id == R.id.btn_request_camera_permission) {
            PermissionUtil.requestCameraPermission(this);
        } else if (id == R.id.btn_capture) {
            Intent intent = new Intent(this, CameraActivity.class);
            startActivity(intent);
        } else if (id == R.id.btn_get_preview_resolution) {
            getPreviewResolution(ParameterType.PREVIEW_SIZE);
        }
    }

    private List<String> getPreviewResolution(ParameterType parameterType) {
        Camera camera = Camera.open();
        Camera.Parameters parameters = camera.getParameters();

        List<String> result = new ArrayList<>();

        List<Camera.Size> sizeList = parameters.getSupportedPreviewSizes();
        for (Camera.Size size : sizeList) {
            SLog.info("SIZE[%d X %d]", size.width, size.height);
        }
        camera.release();

        return result;
    }
}


