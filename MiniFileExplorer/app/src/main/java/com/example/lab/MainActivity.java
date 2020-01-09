package com.example.lab;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Environment;
import android.view.View;

import com.chad.library.adapter.base.BaseQuickAdapter;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    RecyclerView rvList;
    FSAdapter fsAdapter;

    RecyclerView rvPathList;
    DirectoryPathAdapter directoryPathAdapter;

    List<FSItem> fsItemList = new ArrayList<>();
    List<String> directoryPathList = new ArrayList<>();
    File externalStorageDirectory = Environment.getExternalStorageDirectory();
    String externalStoragePath = externalStorageDirectory.getAbsolutePath();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn_test).setOnClickListener(this);
        findViewById(R.id.btn_home).setOnClickListener(this);

        PermissionUtil.requestStoragePermission(this);

        rvList = findViewById(R.id.rv_list);
        fsAdapter = new FSAdapter(R.layout.fs_item, fsItemList);
        fsAdapter.setOnItemClickListener(new BaseQuickAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(BaseQuickAdapter adapter, View view, int position) {
                FSItem fsItem = fsItemList.get(position);

                if (fsItem.type == FSItem.TYPE_FOLDER) {
                    openDirectory(new File(fsItem.absolutePath));
                }
            }
        });
        rvList.setLayoutManager(new LinearLayoutManager(this));
        rvList.setAdapter(fsAdapter);

        rvPathList = findViewById(R.id.rv_path_list);
        directoryPathAdapter = new DirectoryPathAdapter(R.layout.diretory_path_item, directoryPathList);
        directoryPathAdapter.setOnItemClickListener(new BaseQuickAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(BaseQuickAdapter adapter, View view, int position) {
                String absolutePath = directoryPathList.get(position);
                SLog.info("absolutePath[%s]", absolutePath);
                openDirectory(new File(absolutePath));
            }
        });
        rvPathList.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false));
        rvPathList.setAdapter(directoryPathAdapter);

        openHomeDirectory();
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();

        if (id == R.id.btn_test) {
        } else if (id == R.id.btn_home) {
            openHomeDirectory();
        }
    }

    private void openHomeDirectory() {
        openDirectory(externalStorageDirectory);
    }

    private void openDirectory(File directory) {
        String absolutePath = directory.getAbsolutePath();
        directoryPathList.clear();
        SLog.info("externalStoragePath[%s], absolutePath[%s]", externalStoragePath, absolutePath);
        while (absolutePath.length() > externalStoragePath.length()) {
            SLog.info("absolutePath[%s]", absolutePath);

            directoryPathList.add(absolutePath);
            absolutePath = PathUtil.getDirectory(absolutePath);
        }

        Collections.reverse(directoryPathList);
        directoryPathAdapter.setNewData(directoryPathList);


        fsItemList.clear();
        File[] files = directory.listFiles();
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
