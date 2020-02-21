package com.example.lab;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.chad.library.adapter.base.BaseQuickAdapter;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, BaseQuickAdapter.RequestLoadMoreListener {
    RecyclerView rvList;
    SimpleAdapter simpleAdapter;

    int currPage = 0;
    boolean hasMore = true;
    public static final int MAX_PAGE = 3;
    public static final int SCREEN_PER_PAGE = 1;

    List<String> stringList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        SLog.info("onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionUtil.requestStoragePermission(this);

        findViewById(R.id.btn_test).setOnClickListener(this);


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

        if (id == R.id.btn_test) {
            loadData(); // 至少要有1條數據才會觸發LoadMore事件
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
            int start = currPage + 1;
            int end = currPage + SCREEN_PER_PAGE;
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
