package com.example.audiorecord;

import android.content.Context;

public class Util {
    public static int dip2px(Context context, float dipValue) {
        float m = context.getResources().getDisplayMetrics().density;
        int result = (int) (dipValue * m + 0.5f);
        SLog.info("result[%d]", result);

        return result;
    }
}
