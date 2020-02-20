package com.example.lab;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionUtil.requestStoragePermission(this);
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
    }
}
