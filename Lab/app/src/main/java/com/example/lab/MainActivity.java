package com.example.lab;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.chad.library.adapter.base.BaseQuickAdapter;

import java.util.ArrayList;
import java.util.List;


/**
 * loadMore实验
 */
public class MainActivity extends AppCompatActivity implements View.OnClickListener, BaseQuickAdapter.RequestLoadMoreListener {
    RecyclerView rvList;
    SimpleAdapter simpleAdapter;

    int currPage = 0; // 当前加载了多少页数据
    boolean hasMore = true;
    public static int MAX_PAGE = 3; // 数据总数，最初时只有3页数据
    public static final int SCREEN_PER_PAGE = 1;  // 每次加载多少页数据

    List<String> stringList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionUtil.requestStoragePermission(this);

        findViewById(R.id.btn_load_init).setOnClickListener(this);
        findViewById(R.id.btn_new_data).setOnClickListener(this);

        rvList = findViewById(R.id.rv_list);
        rvList.setLayoutManager(new LinearLayoutManager(this));
        simpleAdapter = new SimpleAdapter(R.layout.simple_item, stringList);

        simpleAdapter.setEnableLoadMore(true);
        simpleAdapter.setOnLoadMoreListener(this, rvList);

        rvList.setAdapter(simpleAdapter);
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();

        if (id == R.id.btn_load_init) {
            loadData(); // 至少要有1條數據才會觸發LoadMore事件
        } else if (id == R.id.btn_new_data) {
            MAX_PAGE = 150;  // 增加到150页

            hasMore = true;
            simpleAdapter.setEnableLoadMore(true);

            onLoadMoreRequested(); // 直接调用BaseQuickAdapter.RequestLoadMoreListener::onLoadMoreRequested()方法
        }
    }

    @Override
    public void onLoadMoreRequested() {
        SLog.info("onLoadMoreRequested");

        if (!hasMore) {
            simpleAdapter.setEnableLoadMore(false);
            return;
        }
        loadData();
    }

    private void loadData() {
        SLog.info("loadData");
        if (hasMore) {
            // 定义本次加载数据的起止范围
            int start = currPage + 1;
            int end = currPage + SCREEN_PER_PAGE;
            SLog.info("start[%d], end[%d]", start, end);


            for (int i = start; i <= end; i++) {
                stringList.add(String.valueOf(i));
            }

            hasMore = (end < MAX_PAGE);
            SLog.info("hasMore[%s]", hasMore);

            if (!hasMore) {
                simpleAdapter.loadMoreEnd();
                simpleAdapter.setEnableLoadMore(false);
            }

            simpleAdapter.loadMoreComplete();
            simpleAdapter.setNewData(stringList);

            currPage = end;
        }
    }
}
