package com.example.lab;

import android.content.Context;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback {
    private SurfaceHolder mHolder;
    private Camera mCamera;

    public CameraPreview(Context context) {
        this(context, null);
    }

    public CameraPreview(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CameraPreview(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    private static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open();
        } catch (Exception e) {
            SLog.info("Error!camera is not available");
        }
        return c;
    }

    public void surfaceCreated(SurfaceHolder holder) {
        SLog.info("surfaceCreated");
        mCamera = getCameraInstance();
        try {
            mCamera.setPreviewDisplay(holder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        } catch (IOException e) {
            SLog.info("Error!setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        SLog.info("surfaceDestroyed");
        mHolder.removeCallback(this);
        mCamera.setPreviewCallback(null);
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
    }

    public String getCameraInfo() {
        Camera.Parameters parameters = mCamera.getParameters();

        Camera.Size size = parameters.getPreviewSize();
        SLog.info("width[%d], height[%d]", size.width, size.height);
        return null;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        SLog.info("onPreviewFrame, data length[%d]", data.length);
    }
}
