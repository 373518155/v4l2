package com.example.lab;

import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.hardware.Camera;
import android.os.Environment;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback,
        Camera.PreviewCallback, Camera.AutoFocusCallback {
    private SurfaceHolder mHolder;
    private Camera mCamera;
    private boolean oneShot;

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

        SLog.info("is Activity[%s]", context instanceof Activity);

        // 用于触摸手动对焦
        setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                int x = (int) event.getX();
                int y = (int) event.getY();
                SLog.info("x[%d], y[%s]", x, y);

                focusOnTouch(x, y);

                return false;
            }
        });
    }

    private void focusOnTouch(int x, int y) {
        int surfaceWidth = getWidth();
        int surfaceHeight = getHeight();
        SLog.info("x[%d], y[%d], surfaceWidth[%d], surfaceHeight[%d]", x, y, surfaceWidth, surfaceHeight);


        /*
        相机的对焦区域的坐标范围为 -1000 到 1000
        左上角 (-1000, -1000)
        右上角 (1000, -1000)
        左下角 (-1000, 1000)
        右下角 (1000, 1000)
         */

        // 映射对焦区域
        Rect rect = new Rect(x - 100, y - 100, x + 100, y + 100);
        int left = rect.left * 2000 / surfaceWidth - 1000;
        int top = rect.top * 2000 / surfaceHeight - 1000;
        int right = rect.right * 2000 / surfaceWidth - 1000;
        int bottom = rect.bottom * 2000 / surfaceHeight - 1000;
        // 如果超出了(-1000,1000)到(1000, 1000)的范围，则会导致相机崩溃
        left = left < -1000 ? -1000 : left;
        top = top < -1000 ? -1000 : top;
        right = right > 1000 ? 1000 : right;
        bottom = bottom > 1000 ? 1000 : bottom;
        focusOnRect(new Rect(left, top, right, bottom));
    }


    protected void focusOnRect(Rect rect) {
        if (mCamera != null) {
            Camera.Parameters parameters = mCamera.getParameters(); // 先获取当前相机的参数配置对象
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO); // 设置聚焦模式
            int maxNumFocusAreas = parameters.getMaxNumFocusAreas();
            if (maxNumFocusAreas > 0) {
                List<Camera.Area> focusAreas = new ArrayList<Camera.Area>();
                focusAreas.add(new Camera.Area(rect, 1000));
                parameters.setFocusAreas(focusAreas);
            }
            mCamera.cancelAutoFocus(); // 先要取消掉进程中所有的聚焦功能
            mCamera.setParameters(parameters); // 一定要记得把相应参数设置给相机
            mCamera.autoFocus(this);
        }
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
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFormat(ImageFormat.NV21);  // 设置了这个，预览才是彩色的，不然是黑白的
            Camera.Size size = parameters.getPreviewSize();
            SLog.info("width[%d], height[%d]", size.width, size.height);
            mCamera.setParameters(parameters);

            int rotateDegree = getPreviewRotateDegree();
            mCamera.setDisplayOrientation(rotateDegree);
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
        int previewFormat = parameters.getPreviewFormat();
        SLog.info("previewFormat[%s]", Util.translatePreviewFormat(previewFormat));
        return null;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        // https://blog.csdn.net/tanmengwen/article/details/41412425
        // Android -- 将NV21图像保存成JPEG
        long threadId = Thread.currentThread().getId();
        // SLog.info("onPreviewFrame, threadId[%d], length[%d]", threadId, data.length);
        if (oneShot) {
            Jarbon jarbon = new Jarbon();
            String absoluteFilePath = Environment.getExternalStorageDirectory() + "/1/" + jarbon.format("Ymd-Hisu") + ".nv21";
            SLog.info("absoluteFilePath[%s]", absoluteFilePath);
            File file = new File(absoluteFilePath);
            try {
                FileOutputStream fos = new FileOutputStream(file);
                fos.write(data);
                fos.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            oneShot = false;
        }
    }

    private int getPreviewRotateDegree(){
        int phoneDegree = 0;
        int result = 0;
        //获得手机方向
        int phoneRotate =  ((Activity) getContext()).getWindowManager().getDefaultDisplay().getOrientation();
        //得到手机的角度
        switch (phoneRotate) {
            case Surface.ROTATION_0: phoneDegree = 0; break;  		//0
            case Surface.ROTATION_90: phoneDegree = 90; break;		//90
            case Surface.ROTATION_180: phoneDegree = 180; break;	//180
            case Surface.ROTATION_270: phoneDegree = 270; break;	//270
        }
        //分别计算前后置摄像头需要旋转的角度
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        boolean isFrontCamera = false;
        if(isFrontCamera){
            Camera.getCameraInfo(Camera.CameraInfo.CAMERA_FACING_FRONT, cameraInfo);
            result = (cameraInfo.orientation + phoneDegree) % 360;
            result = (360 - result) % 360;
        }else{
            Camera.getCameraInfo(Camera.CameraInfo.CAMERA_FACING_BACK, cameraInfo);
            result = (cameraInfo.orientation - phoneDegree +360) % 360;
        }
        return result;
    }

    public void takePicture() {
        oneShot = true;
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        SLog.info("onAutoFocus, success[%s]", success);
    }
}
