package com.example.lab;

import android.app.Activity;
import android.content.Context;
import android.view.View;

public class Util {
    public static int dip2px(Context context, float dipValue) {
        float m = context.getResources().getDisplayMetrics().density;
        int result = (int) (dipValue * m + 0.5f);
        SLog.info("result[%d]", result);

        return result;
    }

    /**
     * 进入全屏模式
     * 参考
     * Android 正确进入全屏和退出全屏的姿势
     * https://blog.csdn.net/bobcat_kay/article/details/82794317
     * @param activity
     */
    public static void enterFullScreen(Activity activity) {
        activity.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
    }

    /**
     * 退出全屏模式
     * 参考
     * Android 正确进入全屏和退出全屏的姿势
     * https://blog.csdn.net/bobcat_kay/article/details/82794317
     * @param activity
     */
    public static void exitFullScreen(Activity activity) {
        activity.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);
    }
}
