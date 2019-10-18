package com.example.audiorecord;

import android.support.annotation.Nullable;

/**
 * 通用回调接口
 * Created by zwm
 * 2018/9/23 11:02
 */
public interface CommonCallback {
    /**
     * 成功时的回调
     * @param data
     * @return
     */
    String onSuccess(@Nullable String data);


    /**
     * 失败时的回调
     * @param data
     * @return
     */
    String onFailure(@Nullable String data);
}
