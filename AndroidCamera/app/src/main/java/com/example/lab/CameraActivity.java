package com.example.lab;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.RelativeLayout;

import java.io.FileOutputStream;

public class CameraActivity extends AppCompatActivity implements
        View.OnClickListener, OnTouchFocusListener {
    CameraPreview cameraPreview;

    FocusIndicator focusIndicator;
    Animation animation;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        Util.enterFullScreen(this);

        findViewById(R.id.btn_info).setOnClickListener(this);
        findViewById(R.id.btn_shutter).setOnClickListener(this);
        cameraPreview = findViewById(R.id.camera_preview);
        focusIndicator = findViewById(R.id.focus_indicator);

        animation = AnimationUtils.loadAnimation(this, R.anim.focus_indicator);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {

            }

            @Override
            public void onAnimationEnd(Animation animation) {
                // 显示一段时间后，隐藏
                focusIndicator.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        focusIndicator.setVisibility(View.GONE);
                    }
                }, 3000);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {

            }
        });
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

    @Override
    public void onTouchFocus(int x, int y) {
        int indicatorWidth = (int) getResources().getDimension(R.dimen.focus_indicator_width);
        int indicatorHeight = (int) getResources().getDimension(R.dimen.focus_indicator_height);
        SLog.info("onTouchFocus, x[%d], y[%d], indicatorWidth[%d], indicatorHeight[%d]",
                x, y, indicatorWidth, indicatorHeight);

        RelativeLayout.LayoutParams layoutParams = (RelativeLayout.LayoutParams) focusIndicator.getLayoutParams();
        layoutParams.leftMargin = x - indicatorWidth / 2;
        layoutParams.topMargin = y - indicatorHeight / 2;
        SLog.info("leftMargin[%d], topMargin[%d]", layoutParams.leftMargin, layoutParams.topMargin);
        focusIndicator.setLayoutParams(layoutParams);

        focusIndicator.setVisibility(View.VISIBLE);
        focusIndicator.startAnimation(animation);
    }
}
