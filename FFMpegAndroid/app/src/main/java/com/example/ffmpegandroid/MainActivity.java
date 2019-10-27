package com.example.ffmpegandroid;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.example.ffmpegandroid.jni.Bridge;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn_check_libx264).setOnClickListener(this);
        findViewById(R.id.btn_check_libfdk_aac).setOnClickListener(this);
        findViewById(R.id.btn_test).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_test) {
            // Toast.makeText(this, new Bridge().getAVFormatInfo(), Toast.LENGTH_SHORT).show();
            SLog.info("support[%s]", new Bridge().getAVCodecInfo());
        } else if (id == R.id.btn_check_libx264) {
            SLog.info("exists[%s]", new Bridge().isEncoderExists("libx264"));
            Toast.makeText(this, String.valueOf(new Bridge().isEncoderExists("libx264")), Toast.LENGTH_SHORT).show();
        } else if (id == R.id.btn_check_libfdk_aac) {
            SLog.info("exists[%s]", new Bridge().isEncoderExists("libfdk_aac"));
            Toast.makeText(this, String.valueOf(new Bridge().isEncoderExists("libfdk_aac")), Toast.LENGTH_SHORT).show();
        }
    }
}
