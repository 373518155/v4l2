package com.example.lab;

import android.content.Context;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.lxj.xpopup.core.BottomPopupView;
import com.lxj.xpopup.util.XPopupUtils;

import java.io.File;

import cn.snailpad.easyjson.EasyJSONObject;

public class FileExplorerBottomMenuPopup extends BottomPopupView implements View.OnClickListener {
    Context context;
    String absolutePath;
    SimpleCallback simpleCallback;

    public FileExplorerBottomMenuPopup(@NonNull Context context, String absolutePath, SimpleCallback simpleCallback) {
        super(context);

        this.context = context;
        this.absolutePath = absolutePath;
        this.simpleCallback = simpleCallback;
    }

    @Override
    protected int getImplLayoutId() {
        return R.layout.file_explorer_bottom_menu_popup;
    }

    @Override
    protected void onCreate() {
        super.onCreate();

        findViewById(R.id.btn_delete).setOnClickListener(this);
    }

    //完全可见执行
    @Override
    protected void onShow() {
        super.onShow();
    }

    //完全消失执行
    @Override
    protected void onDismiss() {

    }

    @Override
    protected int getMaxHeight() {
        return (int) (XPopupUtils.getWindowHeight(getContext())*.85f);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();

        if (id == R.id.btn_delete) {
            Toast.makeText(context, "确定要删除吗?", Toast.LENGTH_SHORT).show();

            File file = new File(absolutePath);
            file.delete();

            simpleCallback.onSimpleCall(EasyJSONObject.generate(
                    "action", FileExplorerAction.DELETE.ordinal()
            ).toString());
            dismiss();
        }
    }

}
