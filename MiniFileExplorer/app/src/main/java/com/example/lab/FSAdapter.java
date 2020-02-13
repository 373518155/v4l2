package com.example.lab;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseViewHolder;

import java.util.List;

public class FSAdapter extends BaseQuickAdapter<FSItem, BaseViewHolder> {
    public FSAdapter(int layoutResId, @Nullable List<FSItem> data) {
        super(layoutResId, data);
    }

    @Override
    protected void convert(BaseViewHolder helper, FSItem item) {
        helper.addOnClickListener(R.id.img_info);
        if (item.type == FSItem.TYPE_FOLDER) {
            helper.setImageResource(R.id.img_file, R.drawable.ic_doc_folder);
        } else {
            helper.setImageResource(R.id.img_file, R.drawable.ic_doc_document);
        }

        String filename = PathUtil.getFilename(item.absolutePath);
        helper.setText(R.id.tv_filename, filename)
                .setText(R.id.tv_file_size, Formatter.formatFileSize(item.length))
                .setText(R.id.tv_file_mtime, new Jarbon(item.mtime).format("Y-m-d H:i"));
    }
}
