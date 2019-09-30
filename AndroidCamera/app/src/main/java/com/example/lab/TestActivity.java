package com.example.lab;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;

public class TestActivity extends AppCompatActivity implements View.OnClickListener {
    FocusIndicator focusIndicator;
    Animation animation;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

        findViewById(R.id.btn_test).setOnClickListener(this);

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
        focusIndicator = findViewById(R.id.focus_indicator);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_test) {
            focusIndicator.setVisibility(View.VISIBLE);
            focusIndicator.startAnimation(animation);
        }
    }
}
