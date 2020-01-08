package com.example.lab;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Environment;
import android.view.View;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    RecyclerView rvList;
    FSAdapter fsAdapter;

    List<FSItem> fsItemList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn_test).setOnClickListener(this);

        PermissionUtil.requestStoragePermission(this);

        rvList = findViewById(R.id.rv_list);
        fsAdapter = new FSAdapter(R.layout.fs_item, fsItemList);
        rvList.setLayoutManager(new LinearLayoutManager(this));
        rvList.setAdapter(fsAdapter);
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();

        if (id == R.id.btn_test) {
            fsItemList.clear();

            File storage = Environment.getExternalStorageDirectory();
            File[] files = storage.listFiles();

            for (int i = 0; i < files.length; i++) {
                File file = files[i];
                FSItem fsItem = new FSItem();

                if (file.isDirectory()) {
                    fsItem.type = FSItem.TYPE_FOLDER;
                } else {
                    fsItem.type = FSItem.TYPE_FILE;
                }

                fsItem.absolutePath = file.getAbsolutePath();
                fsItem.length = file.length();
                fsItem.mtime = file.lastModified();

                fsItemList.add(fsItem);
            }

            fsAdapter.setNewData(fsItemList);
        }
    }
}
