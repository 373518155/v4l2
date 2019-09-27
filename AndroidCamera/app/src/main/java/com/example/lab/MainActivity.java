package com.example.lab;

import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.lxj.xpopup.XPopup;
import com.lxj.xpopup.interfaces.OnSelectListener;
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
        findViewById(R.id.btn_get_picture_resolution).setOnClickListener(this);
        findViewById(R.id.btn_get_video_resolution).setOnClickListener(this);
        findViewById(R.id.btn_get_focus_mode).setOnClickListener(this);
        findViewById(R.id.btn_get_picture_format).setOnClickListener(this);
        findViewById(R.id.btn_get_preview_format).setOnClickListener(this);
        findViewById(R.id.btn_get_preview_fps_range).setOnClickListener(this);
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
            String[] result = getParameters(ParameterType.PREVIEW_SIZE);
            showResultPopup("预览分辨率", result, -1);
        } else if (id == R.id.btn_get_picture_resolution) {
            String[] result = getParameters(ParameterType.PICTURE_SIZE);
            showResultPopup("圖片分辨率", result, -1);
        } else if (id == R.id.btn_get_video_resolution) {
            String[] result = getParameters(ParameterType.VIDEO_SIZE);
            showResultPopup("視頻分辨率", result, -1);
        } else if (id == R.id.btn_get_focus_mode) {
            String[] result = getParameters(ParameterType.FOCUS_MODE);
            showResultPopup("對焦模式", result, -1);
        } else if (id == R.id.btn_get_picture_format) {
            String[] result = getParameters(ParameterType.PICTURE_FORMAT);
            showResultPopup("圖片格式", result, -1);
        } else if (id == R.id.btn_get_preview_format) {
            String[] result = getParameters(ParameterType.PREVIEW_FORMAT);
            showResultPopup("預覽格式", result, -1);
        } else if (id == R.id.btn_get_preview_fps_range) {
            String[] result = getParameters(ParameterType.PREVIEW_FPS_RANGE);
            showResultPopup("預覽幀率", result, -1);
        }
    }

    private String[] getParameters(ParameterType parameterType) {
        Camera camera = Camera.open();
        Camera.Parameters parameters = camera.getParameters();

        List<String> result = new ArrayList<>();
        if (parameterType == ParameterType.PREVIEW_SIZE || parameterType == ParameterType.PICTURE_SIZE ||
                parameterType == ParameterType.VIDEO_SIZE) {
            List<Camera.Size> sizeList = null;
            if (parameterType == ParameterType.PREVIEW_SIZE) {
                sizeList = parameters.getSupportedPreviewSizes();
            } else if (parameterType == ParameterType.PICTURE_SIZE) {
                sizeList = parameters.getSupportedPictureSizes();
            } else if (parameterType == ParameterType.VIDEO_SIZE) {
                sizeList = parameters.getSupportedVideoSizes();
            }

            for (Camera.Size size : sizeList) {
                SLog.info("SIZE[%d X %d]", size.width, size.height);
                String item = String.format("%d X %d", size.width, size.height);
                result.add(item);
            }
        } else if (parameterType == ParameterType.FOCUS_MODE) {
            result = parameters.getSupportedFocusModes();
        } else if (parameterType == ParameterType.PICTURE_FORMAT) {
            List<Integer> formatList = parameters.getSupportedPictureFormats();
            for (int format : formatList) {
                String formatDesc = translateImageFormat(format);
                result.add(formatDesc);
            }
        } else if (parameterType == ParameterType.PREVIEW_FORMAT) {
            List<Integer> formatList = parameters.getSupportedPreviewFormats();
            for (int format : formatList) {
                String formatDesc = translatePreviewFormat(format);
                result.add(formatDesc);
            }
        } else if (parameterType == ParameterType.PREVIEW_FPS_RANGE) {
            List<int[]> fpsRangeList = parameters.getSupportedPreviewFpsRange();
            for (int[] fpsRange : fpsRangeList) {
                String item = String.format("%.2f(min) - %.2f(max)", ((float) fpsRange[0]) / 1000, ((float) fpsRange[1]) / 1000);
                result.add(item);
            }
        }
        

        camera.release();
        return stringListToArray(result);
    }

    /**
     * 显示结果弹窗
     * @param title 标题
     * @param data 数据
     * @param checkedPosition 选中哪一项，从0开始，-1表示不选中
     */
    private void showResultPopup(String title, String[] data, int checkedPosition) {
        new XPopup.Builder(this)
                .asCenterList(title, data,
                        null, 1,
                        new OnSelectListener() {
                            @Override
                            public void onSelect(int position, String text) {
                                SLog.info("");
                            }
                        })
                .show();
    }

    private String[] stringListToArray(List<String> stringList) {
        if (stringList == null || stringList.size() < 1) {
            return null;
        }
        String[] stringArray = new String[stringList.size()];

        int index = 0;
        for (String str : stringList) {

            stringArray[index] = str;
            index++;
        }

        return stringArray;
    }

    private String translateImageFormat(int format) {
        SLog.info("format[%d]", format);
        switch (format) {
            case 0x100:
                return "JPEG";
            default:
                return "UNKNOWN";
        }
    }

    private String translatePreviewFormat(int format) {
        SLog.info("format[%d]", format);
        switch (format) {
            case 0x11:
                return "NV21";
            case 0x32315659:
                return "YV12";
            default:
                return "UNKNOWN";
        }
    }
}


