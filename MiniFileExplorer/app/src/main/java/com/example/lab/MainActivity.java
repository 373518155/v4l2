package com.example.lab;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Environment;
import android.view.View;
import android.widget.Toast;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.lxj.xpopup.XPopup;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import cn.snailpad.easyjson.EasyJSONException;
import cn.snailpad.easyjson.EasyJSONObject;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, SimpleCallback {
    RecyclerView rvList;
    FSAdapter fsAdapter;

    RecyclerView rvPathList;
    DirectoryPathAdapter directoryPathAdapter;

    File currDirectory; // 当前所在的路径

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
        findViewById(R.id.btn_go_up).setOnClickListener(this);

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
        fsAdapter.setOnItemChildClickListener(new BaseQuickAdapter.OnItemChildClickListener() {
            @Override
            public void onItemChildClick(BaseQuickAdapter adapter, View view, int position) {
                FSItem fsItem = fsItemList.get(position);
                SLog.info("absolutePath[%s]", fsItem.absolutePath);

                new XPopup.Builder(MainActivity.this)
                        // 如果不加这个，评论弹窗会移动到软键盘上面
                        .moveUpToKeyboard(false)
                        .asCustom(new FileExplorerBottomMenuPopup(MainActivity.this, fsItem.absolutePath, MainActivity.this))
                        .show();
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
        } else if (id == R.id.btn_go_up) {
            boolean success = goUp();
            if (!success) {
                Toast.makeText(this, "已经是最顶层目录", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void openHomeDirectory() {
        openDirectory(externalStorageDirectory);
    }

    private void refresh() {
        openDirectory(currDirectory);
    }

    private void openDirectory(File directory) {
        currDirectory = directory;
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

        // 按名称升序排序
        Arrays.sort(files, new Comparator<File>() {
            @Override
            public int compare(File o1, File o2) {
                String filename1 = o1.getName();
                String filename2 = o2.getName();

                return filename1.compareToIgnoreCase(filename2);
            }
        });

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

    @Override
    public void onSimpleCall(Object data) {
        String dataStr = (String) data;
        EasyJSONObject dataObj = (EasyJSONObject) EasyJSONObject.parse(dataStr);

        try {
            int action = dataObj.getInt("action");
            if (action == FileExplorerAction.DELETE.ordinal()) {
                // 如果是删除了文件/文件夹，则刷新当前目录
                refresh();
            }
        } catch (EasyJSONException e) {
            e.printStackTrace();
        }
    }


    /**
     * 返回上一层目录
     * @return  成功返回true, 失败返回false
     */
    private boolean goUp() {
        String parentPath = currDirectory.getParent();
        SLog.info("externalStoragePath[%s], parentPath[%s]", externalStoragePath, parentPath);

        if (externalStoragePath.length() > parentPath.length()) {
            return false;
        }

        openDirectory(new File(parentPath));
        return true;
    }
}
